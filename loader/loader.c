#include "loader.h"
#include <stdio.h>
#include <signal.h>

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;

int PAGE_SIZE = 4096;
int page_faults = 0;
int page_allocations = 0;
int internal_fragmentation = 0;

void seg_handler(int signum, siginfo_t *info, void *context){
  page_faults++;
  void *fault_address = info->si_addr;
    for (int i = 0; i < ehdr->e_phnum; i++) {
      void *start_address = (void *) phdr[i].p_vaddr;
      void *end_address = start_address + phdr[i].p_memsz;
        if (fault_address >= start_address && fault_address < end_address) {
            void *virtual_mem = mmap(fault_address, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
            if (virtual_mem == MAP_FAILED) {
                perror("Error: memory allocation with mmap");
                close(fd);
                exit (1);
            }

            int offset = phdr[i].p_offset + (uintptr_t)fault_address /PAGE_SIZE * PAGE_SIZE - phdr[i].p_vaddr;
            if (lseek(fd, offset, SEEK_SET) < 0 ) {
                perror("Error: seeking to entrypoint");
                munmap(virtual_mem, phdr->p_memsz);
                close(fd);
                exit (1);
            }

            if (read(fd, virtual_mem, PAGE_SIZE) == -1) {
                perror("Error: reading segment content");
                munmap(virtual_mem, phdr[i].p_memsz);
                close(fd);
                exit (1);
            }

            if (fault_address + PAGE_SIZE > end_address) {
              internal_fragmentation += PAGE_SIZE + fault_address - end_address;
            }
            page_allocations++;
            break;
        }
    }
}

void loader_cleanup() {
    free(ehdr);
    ehdr = NULL;
    free(phdr);
    phdr = NULL;
    close(fd);
}

void setup_seg_handler() {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = seg_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGSEGV, &sa, NULL);
}

void load_and_run_elf(char** exe) {
// 1. Load entire binary content into the memory from the ELF file.
    fd = open(exe[1], O_RDONLY);
    if (fd < 0) {
        perror("Error: opening file");
        exit (1);
    }

    ehdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));
    if (ehdr == NULL) {
        perror("Error: memory allocation for ELF header");
        close(fd);
        exit(1);
    }
    if (read(fd, ehdr, sizeof(Elf32_Ehdr)) != sizeof(Elf32_Ehdr)) {
        perror("Error: reading ELF header");
        close(fd);
        exit (1);
    }

    lseek(fd, ehdr->e_phoff, SEEK_SET);
    phdr = (Elf32_Phdr *)malloc(ehdr->e_phnum * ehdr->e_phentsize);
    if (phdr == NULL) {
        perror("Error: memory allocation for program header");
        close(fd);
        exit(1);
    }
    if (read(fd, phdr, ehdr->e_phnum * ehdr->e_phentsize) != ehdr->e_phnum * ehdr->e_phentsize) {
        perror("Error: reading program header");
        close(fd);
        exit(1);
    }

    setup_seg_handler();

// 4. Navigate to the entrypoint address into the segment loaded in the memory in above step
    int (*_start)(void) = (int (*)(void))(ehdr->e_entry);

// 5. Typecast the address to that of function pointer matching "_start" method in fib.c.
    int result = _start();

// 6. Call the "_start" method and print the value returned from the "_start"
    printf("User _start return value = %d\n",result);

    printf("Total page faults: %d\n", page_faults);
    printf("Total page allocations: %d\n", page_allocations);
    printf("Total internal fragmentation: %f KB\n", (float)internal_fragmentation / 1024.0);

    loader_cleanup();
}