#include "virtual_alloc.h"
#include "virtual_sbrk.h"
#include <stdio.h>
#include <stdlib.h>

// Retreives the block count value
uint64_t get_block_count(uint8_t * partition_table) {
    uint64_t * block_count = (uint64_t *)partition_table;
    return block_count[0];
}

// Sets the block count value
void set_block_count(uint8_t * partition_table, uint64_t size) {
    uint64_t * block_count = (uint64_t *)partition_table;
    block_count[0] = size;
}

// Get the block status (obtain first bit by dividing and rounding)
uint8_t get_status(uint8_t * address) {
    return (* address)/128;
}

// Get the block size (finds remaining bit value)
uint8_t get_space(uint8_t * address) {
    return (* address) % 128;
}

// Set the block status (accepts 0 or 1)
void set_status(uint8_t * address, uint8_t status) {
    if (get_status(address) == 0) {
        * address += 128*status;
    } else {
        * address = (* address - 128) + 128*status;
    }
}

// Set the block size (accepts the exponent)
void set_space(uint8_t * address, uint8_t size) {
    if (get_status(address) == 0) {
        * address = size;
    } else {
        * address = 128 + size;
    }
}

// Finds the value of 2 to the power of "exponent"
uint64_t power_2(uint8_t exponent) {
    if (exponent == 0) { // Bit shifting does not work expectedly with 0
        return 1;
    } 
    return ((uint64_t)2 << (exponent-1));
}

// Finds the exponent of the smallest power of 2 greater than or equal to "size"
uint8_t log_2(uint64_t size) {
    if (size == 1) { // Bit shifting does not work expectedly with 0
        return 0;
    } 
    for (int i = 0; i < size; i++) {
        if (power_2(i) >= size) {
            return i;
        }
    }
    return 0;
}

// Inserts a new block in the partition table, causing each element starting
// at the current "index" to be shifted right by one.
int insert_block(uint8_t * partition_table, uint64_t index, uint8_t status, 
    uint8_t size) {
    uint64_t block_count = get_block_count(partition_table); 
    
    if (virtual_sbrk(1) == (void *)(-1)) {
        return 0; 
    } else if (index > block_count) {
        return 0;
    }

    // Shift everything RIGHT, starting from the end
    for (int i = block_count; i > index; i--) { 
        partition_table[8 + i] = partition_table[8 + i-1]; 
    }
    partition_table[8 + index] = 128*status + size; // Insert new block data
    set_block_count(partition_table, get_block_count(partition_table)+1);
    return 1;
}

// Removes a block in the partition table, causing the elements right of the
// "index" to be shifted left by one.
int remove_block(uint8_t * partition_table, uint64_t index) {
    uint64_t block_count = get_block_count(partition_table); 
    if (index >= block_count) {
        return 0;
    }

    // Shift everything LEFT, starting from the index of removal
    for (int i = index; i < block_count; i++) { 
        partition_table[8 + i] = partition_table[8 + i+1];
    }
    set_block_count(partition_table, get_block_count(partition_table)-1);
    // Decrease the program_break by 1 (technically this cannot return an error)
    virtual_sbrk(-1);
    
    return 1;
}

// ____________________________________________________________________________

// The location of the program break will always be minimum, fitting exactly
// to the amount of storage needed for the partition table (exactly next byte)

// Setup buddy allocator, starts with a 2^initial_size (bytes) memory block for
// allocation. The minimum size of allocatable blocks will be 2^min_size (bytes)
// (These initial values are fixed and cannot be altered later)
// [The "theoretical maximum" size of the virtual memory is 2^63 bytes]
void init_allocator(void * heapstart, uint8_t initial_size, uint8_t min_size) {
    if (heapstart == NULL || initial_size < min_size || initial_size >= 64) {
        printf("The provided sizes cannot be initialised.\n");
        exit(1);
    } 
    uint8_t * heapstart_ptr = (uint8_t *)heapstart;
    void * program_break = virtual_sbrk(0);
    if (program_break == (void *)(-1)) {
        exit(1);
    }
    uint64_t allocatable_space = power_2(initial_size);

    // If the size of memory space is smaller than what we need, increase 
    // program_break. (Starting size of buddy allocator is 3+8)
    int64_t difference = (program_break - heapstart) - (allocatable_space + 3+8);
    if (difference < 0) {
        // Increase program_break by the difference we need, and check error
        if (virtual_sbrk(-difference) == (void *)(-1)) { 
            exit(1);
        }
    }

    // Since virtual_info is needed, a table of all the partitions will be stored
    // Inside the virtual_heap:
    // - The first 2 bytes store the initialisation parameters.
    // - The next (allocatable_space) bytes is for the memory
    // - The next (8 + number of blocks) bytes is for the partition table
    //    - The first (8) bytes represents the total number of blocks
    //    - After will be sets of (1) byte each describing a partition
    //        - Leftmost bit (big endian) will be 0 for free, 1 for allocated
    //        - Remaining bits will represent the 2's exponent of the block's size
    // (If block value is >= 128 (8th bit), it is allocated)
    heapstart_ptr[0] = initial_size; 
    heapstart_ptr[1] = min_size;
    set_block_count(heapstart_ptr + 2 + allocatable_space, 1); // 1 Block
    heapstart_ptr[2 + allocatable_space + 8] = initial_size; // Free + 2^Size
}

// Request an allocation of "size" bytes
void * virtual_malloc(void * heapstart, uint32_t size) {
    if (heapstart == NULL || size == 0) {
        return NULL;
    }
    uint8_t * heapstart_ptr = (uint8_t *)heapstart;
    uint64_t allocatable_space = power_2(heapstart_ptr[0]);
    uint8_t min_size = heapstart_ptr[1];
    if (size > allocatable_space) {
        return NULL;
    }

    // Buddy Allocator Algorithm
    uint8_t * partition_table = heapstart_ptr + 2 + allocatable_space;
    uint64_t block_count = get_block_count(partition_table);
    uint8_t exponent = log_2(size); // Nearest exponent of the requested size

    // If allocation has not occurred, conduct splitting of blocks. Iterate up
    // from the value of the initial exponent up to initial_size. If a block is
    // found of any size larger than the exponent, stop iteration and split. 
    // (If found, this guarantees a space for the allocation as its exact address)
    
    if (exponent < min_size) {
        exponent = min_size;  
        // If exponent is smaller than min_size, treat is as min_size
    }
    void * current_address;
    for (int exp_i = exponent; exp_i <= heapstart_ptr[0]; exp_i++) {
        current_address = heapstart + 2; // Set initial address to first block
        // Iterate through all blocks starting from left
        for (int count = 0; count < block_count; count++) {
            uint8_t status = get_status(&partition_table[8 + count]);
            uint8_t space = get_space(&partition_table[8 + count]);
            if (status == 0) { // Free Block
                // If block size is equal to the exponent iterator, conduct split
                // k times equal to the difference to the actual exponent
                // (e.g. if difference of 2 in 2^2 and 2^4, split occurs twice)
                if (exp_i == space) { 
                    uint8_t split_size = space;
                    // Check the number of splits is possible within program_break
                    uint8_t difference = exp_i - exponent;
                    if (virtual_sbrk(difference) == (void *)(-1)) {
                        return NULL;
                    } else {
                        virtual_sbrk(-(difference));
                    }

                    for (int i = 0; i < difference; i++) {
                        // The current block has its size "halved" and shifted
                        // right. The buddy block is inserted at its current
                        // place
                        split_size -= 1;
                        set_space(&partition_table[8 + count], split_size);
                        insert_block(partition_table, count, 0, split_size);
                    }
                    set_status(&partition_table[8 + count], 1);
                    return current_address; // Allocation address (very left)
                }
            }
            current_address += power_2(space);
        } 
    }
    return NULL;
}

int virtual_free(void * heapstart, void * ptr) {
    if (heapstart == NULL || ptr == NULL) {
        return 1;
    }
    uint8_t * heapstart_ptr = (uint8_t *)heapstart;
    uint64_t allocatable_space = power_2(heapstart_ptr[0]);
    uint8_t * partition_table = heapstart_ptr + 2 + allocatable_space;
    uint64_t block_count = get_block_count(partition_table);

    // All blocks except for the first have a buddy block, but that may be split,
    // which makes it difficult to interpret which side is the buddy. 
    // [Imagine] the entire memory is split into blocks the size of the pointer's
    // block. If we find the actual pointer block's index given this setting:
    // - EVEN (and 0) index means the buddy is on the RIGHT
    // - ODD index means the buddy is in the LEFT

    // Understanding this, traverse to find whether the pointer provided
    // points to an existing block. If so, (address-start) and divide that by 
    // the block's size to obtain the index in the above setting.
    // If the buddy block is free, combine + repeat the process up to initial_size

    void * current_address = heapstart + 2; // Set initial to first block
    for (int count = 0; count < block_count; count++) {
        uint8_t status = get_status(&partition_table[8 + count]);
        uint8_t space = get_space(&partition_table[8 + count]);
        if (current_address == ptr) { // If the address exists, begin reformation
            if (status == 0) {
                return 1; // Error, the address block is currently freed
            }
            set_status(&partition_table[8 + count], 0); // Mark space as freed
            uint8_t combine_size = space;
            uint64_t position_index = 0;
            while (combine_size < heapstart_ptr[0]) {
                // End once merging reaches the large block
                if (combine_size == allocatable_space) { 
                    break;
                }
                // Positional index given description setting
                position_index = ((current_address - (heapstart+2)) /
                    power_2(combine_size));
                combine_size += 1;
                // If the buddy block is either allocated or not the same size
                // end the reformation process.
                // Process deletes the buddy block, then increases the size
                // of the original to become to parent block
 
                // If the block is allocated or not the same size, end process
                if (position_index % 2 == 0) { // EVEN Index --> RIGHT
                    if (get_status(&partition_table[8 + count + 1]) == 1 || 
                        get_space(&partition_table[8 + count + 1]) != combine_size-1) {
                        return 0;
                    }
                    // Remove the right buddy block, and alter the original
                    remove_block(partition_table, count+1);
                    set_space(&partition_table[8 + count], combine_size);
                    // [Same parent block parameters for the next iteration]                
                } else { // ODD Index --> LEFT
                    if (get_status(&partition_table[8 + count - 1]) == 1 || 
                        get_space(&partition_table[8 + count - 1]) != combine_size-1) {
                        return 0;
                    }
                    // Remove the right buddy block, and alter the original
                    remove_block(partition_table, count-1);

                    // [Change the parameters of the new parent shfited left]
                    set_space(&partition_table[8 + count - 1], combine_size);
                    // Decrement by the size of the removed buddy block
                    current_address -= power_2(combine_size-1);
                    // Change the reference to the shifted block
                    count -= 1;
                }
            }
            return 0;
        }
        current_address += power_2(space);
    }
    return 1; // The address is not found inside the partition table
}

void * virtual_realloc(void * heapstart, void * ptr, uint32_t size) {
    if (heapstart == NULL) {
        return NULL;
    } else if (size == 0) { // Behave as virtual_free(ptr) was called, return NULL
        virtual_free(heapstart, ptr);
        return NULL;
    } else if (ptr == NULL) { // Behave as though virtual_malloc(size) was called
        return virtual_malloc(heapstart, size);
    }
    // If realloc fails the original space must not be altered, so duplicate the
    // table temporarily at the current program_break, to be then removed later
    uint8_t * heapstart_ptr = (uint8_t *)heapstart;
    uint64_t allocatable_space = power_2(heapstart_ptr[0]);
    uint8_t * partition_table = heapstart_ptr + 2 + allocatable_space;
    uint64_t block_count = get_block_count(partition_table);
    if (size > allocatable_space) {
        return NULL;
    }

    // Increase program break by the size of the current table
    // Free() and Malloc() failure itself ensures no changes are made to
    // the current table. 
    // - The former only removes partitions, so alterations will not override
    //   data in the new table
    // - If the latter is successful, the save table is now irrelevant to be
    //   altered or not
    uint8_t * new_table = virtual_sbrk(8 + block_count);
    if (new_table == (void *)(-1)) {
        return NULL;
    }
    // Duplicate the block count
    set_block_count(new_table, block_count);
    for (int i = 1; i < block_count; i++) {
        new_table[i] = partition_table[i];
    }
    void * new_ptr = NULL;
    // Conduct Free() and if successful conduct Malloc() as well
    int exit_code = virtual_free(heapstart, ptr);
    if (exit_code == 0) { // If exit code is not 0, return NULL at the end
        new_ptr = virtual_malloc(heapstart, size);
        // Revert the partition table
        if (new_ptr == NULL) {
            set_block_count(partition_table, get_block_count(new_table));
            for (int i = 1; i < get_block_count(partition_table); i++) {
                partition_table[i] = new_table[i];
            }
        // Copy data from the original to the new space if different addresses
        } else if (new_ptr != ptr) {
            // Copy all values up to the new block's size
            for (int i = 0; i < size; i++) {
                if ((void *)ptr+i >= (void *)partition_table) { 
                    break; // If data reading exceeds allocatable memory
                }
                * (uint8_t *)(new_ptr+i) = * (uint8_t *)(ptr+i);
            }
        }

    } 
    // Revert the program break to remove the temporary table
    // Malloc could've extended into the saved table, so find the difference
    uint64_t difference = (8 + get_block_count(partition_table)) - 
                          (8 + block_count);
    virtual_sbrk(-(8 + get_block_count(partition_table) - difference));
    return new_ptr;
}

// Output a list of the state of all blocks
void virtual_info(void * heapstart) {
    if (heapstart == NULL) {
        return;
    }
    uint8_t * heapstart_ptr = (uint8_t *)heapstart;
    uint64_t allocatable_space = power_2(heapstart_ptr[0]);
    uint64_t block_count = get_block_count(heapstart_ptr + 2 + allocatable_space);
    for (int i = 0; i < block_count; i++) {
        uint8_t status = get_status(&heapstart_ptr[2 + allocatable_space + i + 8]);
        uint8_t space = get_space(&heapstart_ptr[2 + allocatable_space + i + 8]);
        uint64_t size = power_2(space);
        if (status == 0) {
            printf("free ");
        } else if (status == 1) {
            printf("allocated ");
        }
        printf("%lu\n", size);
    }
}