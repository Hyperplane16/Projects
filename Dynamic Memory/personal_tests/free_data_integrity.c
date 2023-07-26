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

    init_allocator(virtual_heap, 10, 0);
    uint8_t * ptr1 = (uint8_t *)virtual_malloc(virtual_heap, 50);
    for (int i = 0; i < 50; i++) {
        ptr1[i] = 50;
    }

    // Consider the veracity of data of PTR2
    uint8_t * ptr2 = (uint8_t *)virtual_malloc(virtual_heap, 200);
    uint8_t save[200];
    for (int i = 0; i < 200; i++) {
        ptr2[i] = 200 - i;
        save[i] = 200 - i;
    }

    uint8_t * ptr3 = (uint8_t *)virtual_malloc(virtual_heap, 2);
    void * ptr4 = virtual_malloc(virtual_heap, 500);
    uint8_t * ptr5 = (uint8_t *)virtual_malloc(virtual_heap, 16);
    for (int i = 0; i < 16; i++) {
        ptr1[5] = i*2;
    }

    virtual_free(virtual_heap, ptr3);
    ptr3 = virtual_malloc(virtual_heap, 8);
    for (int i = 0; i < 8; i++) {
        ptr3[i] = 255;
    }
    virtual_free(virtual_heap, ptr5);
    virtual_free(virtual_heap, ptr3);
    ptr3 = virtual_malloc(virtual_heap, 1);
    virtual_free(virtual_heap, ptr3);
    virtual_free(virtual_heap, ptr4);
    virtual_free(virtual_heap, ptr1);
    
    virtual_info(virtual_heap);
    for (int i = 0; i < 200; i++) { // Test the data in PTR2
        if (ptr2[i] != save[i]) {
            printf("User data altered\n");
        }
    }

    free(virtual_heap);
    return 0;
}