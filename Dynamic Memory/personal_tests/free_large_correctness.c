#include "../virtual_alloc.h"
#include "test_virtual_sbrk.h"

int main() {
    total_size = ((uint64_t)2 << (24-1));
    virtual_heap = malloc(sizeof(uint8_t)*total_size);
    if (virtual_heap == NULL) {
        printf("Virtual heap allocation has failed.\n");
        return 1;
    }
    program_break = virtual_heap;

    init_allocator(virtual_heap, 23, 10);
    // Conduct malloc 2000 times
    for (int i = 0; i < 2000; i++) {
        virtual_malloc(virtual_heap, 2048);
    }

    // Free all blocks except the first
    for (int i = 1; i < 2000; i++) {
        virtual_free(virtual_heap, virtual_heap+2+(i*2048));
    }
    virtual_info(virtual_heap);
    
    free(virtual_heap);
    return 0;
}