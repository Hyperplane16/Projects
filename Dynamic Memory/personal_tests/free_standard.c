#include "../virtual_alloc.h"
#include "test_virtual_sbrk.h"

int main() {
    total_size = ((uint64_t)2 << (12-1));
    virtual_heap = malloc(sizeof(uint8_t)*total_size);
    if (virtual_heap == NULL) {
        printf("Virtual heap allocation has failed.\n");
        return 1;
    }
    program_break = virtual_heap;

    init_allocator(virtual_heap, 10, 5);
    void * ptr1 = virtual_malloc(virtual_heap, 70);
    void * ptr2 = virtual_malloc(virtual_heap, 35);
    void * ptr3 = virtual_malloc(virtual_heap, 80);
    virtual_info(virtual_heap);
    printf("\n");

    virtual_free(virtual_heap, ptr1);
    virtual_info(virtual_heap);
    printf("\n");

    void * ptr4 = virtual_malloc(virtual_heap, 60);
    virtual_info(virtual_heap);
    printf("\n");

    virtual_free(virtual_heap, ptr2);
    virtual_info(virtual_heap);
    printf("\n");

    virtual_free(virtual_heap, ptr4);
    virtual_info(virtual_heap);
    printf("\n");

    virtual_free(virtual_heap, ptr3);
    virtual_info(virtual_heap);
    
    free(virtual_heap);
    return 0;
}