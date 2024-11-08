#include "../loader/loader.h"

int main(int argc, char** argv) {
    if(argc != 2) {
        printf("Usage: %s <ELF Executable> \n",argv[0]);
        exit(1);
    }
// 1. carry out necessary checks on the input ELF file
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("Error: Failed to open ELF file");
        exit(1);
    }
    close(fd);

// 2. passing it to the loader for carrying out the loading/execution
    load_and_run_elf(argv);
    
// 3. invoke the cleanup routine inside the loader
    loader_cleanup();
    return 0;
}
