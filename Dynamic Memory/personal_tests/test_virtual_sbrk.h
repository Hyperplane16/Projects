// -------------------------------------
// virtual_sbrk implementation for tests
// -------------------------------------

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void * virtual_heap = NULL;
void * program_break = NULL;
uint64_t total_size = 0;

// sbrk: a Unix syscall to change the data segement's size. Will alter the virtual 
// program break by the size of "increment." If successful, returns the previous
// program break before incrementation, while using sbrk(0) returns the current
void * virtual_sbrk(int32_t increment) {
    if (program_break + increment - virtual_heap > total_size) {
        printf("Required program break modification exceeds the virtual memory.\n");
        return (void *)(-1);
    }
    program_break += increment;
    return program_break-increment;
}
