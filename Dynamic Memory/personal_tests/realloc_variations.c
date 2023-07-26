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
    // Realloc a NULL, in this case act as malloc()
    void * ptr2 = virtual_realloc(virtual_heap, NULL, 500);

    virtual_info(virtual_heap);
    printf("\n");

    // Realloc a size 0, act as free()
    void * ptr3 = virtual_realloc(virtual_heap, ptr1, 0);
    if (ptr3 != NULL) {
        printf("Not NULL\n");
    }

    virtual_info(virtual_heap);

    // Realloc a size 0 and NULL, act as free()
    ptr1 = virtual_realloc(virtual_heap, NULL, 0);
    if (ptr1 != NULL) {
        printf("Not NULL\n");
    }

    free(virtual_heap);
    return 0;
}