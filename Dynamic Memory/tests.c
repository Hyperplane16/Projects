#include "virtual_alloc.h"
#include "virtual_sbrk.h"
#include <stdio.h>
#include <stdlib.h>

void * virtual_heap = NULL;
void * program_break = NULL;
uint64_t total_size = 0;

// sbrk: a Unix syscall to change the data segment's size. Will alter the virtual 
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

int main() {
    total_size = ((uint64_t)2 << (14-1)); // 2^n = 2 << (n-1) (total size of virtual memory)
    virtual_heap = malloc(sizeof(uint8_t)*total_size);
    if (virtual_heap == NULL) {
        printf("Virtual heap allocation has failed.\n");
        return 1;
    }
    program_break = virtual_heap; // Set program_break to the start
 
    free(virtual_heap);
    return 0;
}
