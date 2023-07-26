#include "../virtual_alloc.h"
#include "test_virtual_sbrk.h"

// Tests that the program_break is correctly incremented and decremented
// to perfectly fit the buddy allocator overhead after a series of allocations

int main() {
    total_size = ((uint64_t)2 << (12-1));
    virtual_heap = malloc(sizeof(uint8_t)*total_size);
    if (virtual_heap == NULL) {
        printf("Virtual heap allocation has failed.\n");
        return 1;
    }
    program_break = virtual_heap;

    init_allocator(virtual_heap, 10, 0);    
    void * ptr1 = virtual_malloc(virtual_heap, 50);
    void * ptr2 = virtual_malloc(virtual_heap, 100);
    void * ptr3 = virtual_malloc(virtual_heap, 512);
    void * ptr4 = virtual_realloc(virtual_heap, ptr1, 200);
    
    virtual_info(virtual_heap);
    printf("\n");
    if (program_break != virtual_heap+2+1024+8+4) { // 4 blocks
        printf("Program break in incorrect location\n");
        return 1;
    }

    virtual_free(virtual_heap, ptr2);
    virtual_free(virtual_heap, ptr4);
    ptr2 = virtual_realloc(virtual_heap, ptr3, 1);
    
    virtual_info(virtual_heap);
    printf("\n");
    if (program_break != virtual_heap+2+1024+8+11) { // 11 blocks
        printf("Program break in incorrect location\n");
        return 1;
    }
    

    virtual_realloc(virtual_heap, ptr2, 0); // Act as Free

    virtual_info(virtual_heap);
    if (program_break != virtual_heap+2+1024+8+1) { // 1 block
        printf("Program break in incorrect location\n");
        return 1;
    }
    
    

    free(virtual_heap);
    return 0;
}