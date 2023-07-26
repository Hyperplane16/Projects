#include "../virtual_alloc.h"
#include "test_virtual_sbrk.h"

int main() {
    total_size = ((uint64_t)2 << (16-1));
    virtual_heap = malloc(sizeof(uint8_t)*total_size);
    if (virtual_heap == NULL) {
        printf("Virtual heap allocation has failed.\n");
        return 1;
    }
    program_break = virtual_heap;

    init_allocator(virtual_heap, 15, 10);
    void * ptr1 = virtual_malloc(virtual_heap, 8000);
    void * ptr2 = virtual_malloc(virtual_heap, 10000);
    void * ptr3 = virtual_malloc(virtual_heap, 3000);
    void * ptr4 = virtual_malloc(virtual_heap, 1000);
    void * ptr5 = virtual_malloc(virtual_heap, 1);
    virtual_info(virtual_heap);

    free(virtual_heap);
    return 0;
}