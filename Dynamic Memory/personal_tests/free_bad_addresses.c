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
    void * ptr1 = virtual_malloc(virtual_heap, 200);

    // Null pointer
    printf("%d\n", virtual_free(virtual_heap, NULL));
    // Block currently free
    printf("%d\n", virtual_free(virtual_heap, ptr1 + 512));

    virtual_info(virtual_heap);
    printf("\n");

    // Standard free
    printf("%d\n", virtual_free(virtual_heap, ptr1));
    virtual_info(virtual_heap);
    printf("\n");

    // Trying to re-free
    printf("%d\n", virtual_free(virtual_heap, ptr1));
    virtual_info(virtual_heap);
    
    free(virtual_heap);
    return 0;
}