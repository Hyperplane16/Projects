#include "../virtual_alloc.h"
#include "test_virtual_sbrk.h"

// Tests creating more blocks that possible past the virtual break

int main() {
    total_size = ((uint64_t)2 << (4-1))+1; // 17 Bytes
    virtual_heap = malloc(sizeof(uint8_t)*total_size);
    if (virtual_heap == NULL) {
        printf("Virtual heap allocation has failed.\n");
        return 1;
    }
    program_break = virtual_heap;

    init_allocator(virtual_heap, 2, 0); // 4 Bytes, Min 1 Byte
    // 10 bytes fixed + 4 bytes = 14 bytes
    // 4 --> 1 1 2 (17 bytes total)
    void * ptr1 = virtual_malloc(virtual_heap, 1);
    void * ptr2 = virtual_malloc(virtual_heap, 1);

    // 4 --> 1 1 1 1 (18 bytes total, 1 larger than virtual memory)
    void * ptr3 = virtual_malloc(virtual_heap, 1);
    
    free(virtual_heap);
    return 0;
}