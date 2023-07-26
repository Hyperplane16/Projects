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
    void * ptr1 = virtual_malloc(virtual_heap, 50);
    void * ptr2 = virtual_malloc(virtual_heap, 100);

    virtual_info(virtual_heap);
    printf("\n");

    void * ptr3 = virtual_realloc(virtual_heap, ptr1, 100);
    if (ptr1 != ptr3) {
        printf("Realloc address incorrectly changed.\n");
    }

    virtual_info(virtual_heap);
    printf("\n");

    ptr1 = virtual_malloc(virtual_heap, 500);
    void * ptr4 = virtual_realloc(virtual_heap, ptr2, 200);
    if (ptr4 == ptr2) {
        printf("Realloc address incorrectly changed.\n");
    }

    virtual_info(virtual_heap);
    printf("\n");

    virtual_free(virtual_heap, ptr4);
    ptr4 = virtual_realloc(virtual_heap, ptr3, 400);
    if (ptr4 != ptr3) {
        printf("Realloc address incorrectly changed.\n");
    }

    virtual_info(virtual_heap);

    
    free(virtual_heap);
    return 0;
}