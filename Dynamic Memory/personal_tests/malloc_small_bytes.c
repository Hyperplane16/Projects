#include "../virtual_alloc.h"
#include "test_virtual_sbrk.h"

// Testing allocation of 1 byte blocks

int main() {
    total_size = ((uint64_t)2 << (6-1)); // 64 Bytes
    virtual_heap = malloc(sizeof(uint8_t)*total_size);
    if (virtual_heap == NULL) {
        printf("Virtual heap allocation has failed.\n");
        return 1;
    }
    program_break = virtual_heap;

    init_allocator(virtual_heap, 5, 0); // 32 Bytes, Min 1 Byte
    void * ptr1 = virtual_malloc(virtual_heap, 1);
    void * ptr2 = virtual_malloc(virtual_heap, 1);
    void * ptr3 = virtual_malloc(virtual_heap, 4);
    void * ptr4 = virtual_malloc(virtual_heap, 2);
    void * ptr5 = virtual_malloc(virtual_heap, 1);
    virtual_info(virtual_heap);

    free(virtual_heap);
    return 0;
}