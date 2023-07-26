#include "../virtual_alloc.h"
#include "test_virtual_sbrk.h"

int main() {
    total_size = ((uint64_t)2 << (13-1));
    virtual_heap = malloc(sizeof(uint8_t)*total_size);
    if (virtual_heap == NULL) {
        printf("Virtual heap allocation has failed.\n");
        return 1;
    }
    program_break = virtual_heap;

    // Consider the veracity of data of PTR2 and PTR3
    init_allocator(virtual_heap, 10, 6);

    void * ptr1 = virtual_malloc(virtual_heap, 1);

    uint8_t save2[200];
    uint8_t * ptr2 = (uint8_t *)virtual_malloc(virtual_heap, 50);
    for (int i = 0; i < 50; i++) {
        ptr2[i] = 37 + i;
        save2[i] = 37 + i;
    }

    virtual_free(virtual_heap, ptr1);

    uint8_t * ptr3 = (uint8_t *)virtual_malloc(virtual_heap, 200);
    uint8_t save3[200];
    for (int i = 0; i < 200; i++) {
        ptr3[i] = 200 - i;
        save3[i] = 200 - i;
    }

    virtual_info(virtual_heap);
    printf("\n");

    // Realloc freed but cannot fulfill malloc of given size PTR2
    void * ptr4 = virtual_realloc(virtual_heap, ptr2, 1025);
    if (ptr4 != NULL) {
        printf("Not NULL\n");
    }
    for (int i = 0; i < 50; i++) { // Test the data in PTR2
        if (ptr2[i] != save2[i]) {
            printf("User data altered\n");
        }
    }

    virtual_info(virtual_heap);
    printf("\n");

    // Reallocate and relocate the data PTR2
    uint8_t * ptr5 = (uint8_t *)virtual_realloc(virtual_heap, ptr2, 100);
    for (int i = 0; i < 50; i++) { // Test the data in PTR2
        if (ptr5[i] != save2[i]) {
            printf("User data altered\n");
        }
    }

    virtual_info(virtual_heap);
    printf("\n");

    // Reallocate and relocate the data PTR5
    ptr2 = (uint8_t *)virtual_realloc(virtual_heap, ptr5, 500);
    for (int i = 0; i < 50; i++) { // Test the data in PTR5
        if (ptr2[i] != save2[i]) {
            printf("User data altered\n");
        }
    }

    virtual_info(virtual_heap);
    printf("\n");

    // Reallocate and relocate the data PTR3 
    ptr5 = (uint8_t *)virtual_realloc(virtual_heap, ptr3, 300);
    for (int i = 0; i < 200; i++) { // Test the data in PTR3
        if (ptr5[i] != save3[i]) {
            printf("User data altered\n");
        }
    }

    virtual_info(virtual_heap);
    printf("\n");

    // Reallocate the data PTR5 with a smaller size
    ptr3 = (uint8_t *)virtual_realloc(virtual_heap, ptr5, 50);
    for (int i = 0; i < 50; i++) { // Test the data in PTR5
        if (ptr3[i] != save3[i]) {
            printf("User data altered\n");
        }
    }

    virtual_info(virtual_heap);
    printf("\n");

    // Alter the original PTR2 data currently in the last 512 block
    // then reallocate and and relocate the data
    for (int i = 0; i < 200; i++) {
        ptr2[i] = 255-i;
        save2[i] = 255-i;
    }

    ptr5 = (uint8_t *)virtual_realloc(virtual_heap, ptr2, 100);
    for (int i = 0; i < 100; i++) { // Test the data in PTR2
        if (ptr5[i] != save2[i]) {
            printf("User data altered\n");
        }
    }

    virtual_info(virtual_heap);

    free(virtual_heap);
    return 0;
}