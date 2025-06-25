# Simple Smart Loader

## Description:
The SimpleSmartLoader is an ELF loader written in C that facilitates the loading and execution of 32-bit ELF executables. It operates as a shared library (`lib_simpleloader.so`), offering essential functionalities to map ELF segments into memory page by page and execute the loaded program. This implementation avoids using external ELF manipulation libraries, focusing on low-level operations to achieve its objectives.


## Implementation:
a) We initialize a pointer ehdr (of structure Elf32_Ehdr) representing the ELF header, a pointer phdr (of structure Elf32_Phdr) representing the program header, and a file descriptor.

b) loader_cleanup function frees up the memory allocated. It helps prevent memory leaks and ensures the file is closed properly.

c) load_and_run function loads and executes the ELF file. Opening file, memory allocation, reading ELF header, seeking to phoff and reading program header is done and all intermediate errors are handled.

d) A function pointer is declared. In  that, the _start function calls the entry point process of the ELF file. 

e) The memory allocation takes place lazily, and the received value is returned.

f) The SIGSEGV signal is caught by the page fault handler. The segment in PHT with the fault address is found by iteration, memory of 4KB (page size) is allocated, and any internal fragmentation is reported.

g) Returned value and page fault stats are outputted.

h) Loader cleanup is called.

## To run the project:
`make`<br>
`LD_LIBRARY_PATH=./bin/ ./bin/launch ./test/fib`<br>
`LD_LIBRARY_PATH=./bin/ ./bin/launch ./test/sum`<br>


## Contributions:
Aakanksha (2023004) - defined page fault handler including arithmetic logic, loader_cleanup, memory allocation of and copying the segment content, error handling, make files

Palak Yadav (2023363) - setup sigaction, output stats, loading binary content into the memory, navigating to the entrypoint address, typecasting the address, error handling, make files
