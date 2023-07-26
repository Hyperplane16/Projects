#include "header_x2017.h"

// Calculates value of a base raised to a positive exponent
int power(int base, int exponent) {
    int total = 1;
    for (int i = 0; i < exponent; i++) {
        total = total * base;
    }
    return total;
}

// Convert a subarray of bits in the bit array to its binary equivalent
// Usage: start = index-n to reference the n-th last index and n for subarray size
BYTE convert_binary(BIT * bitp, int start, int size) {
    BYTE decimal = 0;
    if (start < 0) {
        // Returns an error if access becomes illegal
        fprintf(stderr, "Malformed binary in file.\n");
        free(bitp);
        exit(1);
    }
    for (int i = start; i < start + size; i++) {
        decimal += (bitp[i] * power(2, (size-1-(i-start))));
    }
    return decimal;
}

// Given the TYPE of the field, sets the instruction struct value and updates the
// index to be the start of the VALUE field
// (Index supplied must be the end of the VALUE field)
int store_instruction(BIT * bitp, BYTE * type, BYTE * value, int index) {
    if (* type == 0) { // Value (8 bits)
        index = index-8;
        * value = convert_binary(bitp, index, 8);
    } else if (* type == 1) { // Register Address (3 bits)
        index = index-3;
        * value = convert_binary(bitp, index, 3);
    } else if (* type == 2 || * type == 3) { // Stack Symbol or Pointer Value (5 bits)
        index = index-5;
        * value = convert_binary(bitp, index, 5);
    }
    return index;
}

// Store each raw STK value in a table at the index of their appearance. New STK 
// values are either found in the table with the index returned, or added to the end
BYTE add_symbol_table(BYTE * symbol_table, BYTE * symbol_count, BYTE value) {
    BYTE found = 0;
    BYTE store_i = 0;
    while (store_i < * symbol_count) { // Find if it exists in the table
        if (symbol_table[store_i] == value) {
            found = 1;
            break;
        }
        store_i += 1;
    }
    if (found == 0) { // If not found, add it to the end
        symbol_table[* symbol_count] = value;
        * symbol_count += 1;
    }
    return store_i;
}


// For each function frame set the unique stack addresses in its order of appearance 
// to its index of appearance 
// (e.g. STK 10010, if the first to appear, will be set to 00000)
void order_stack_addresses(struct function_frame * func) {  
    BYTE symbol_table[32]; // Number of stack symbols cannot exceed 32 instructions
    BYTE symbol_count = 0; // Reset the symbol table to 0 each function frame

    for (int i = 0; i < func->instruction_count; i++) {
        struct instruction * inst = &func->instruction_list[i];
        // As a field of a pointer is being accessed a double pointer is necessary
        if (inst->first_type == 2 || inst->first_type == 3) {
            inst->first_value = add_symbol_table(symbol_table, &symbol_count, 
                inst->first_value);
        } 
        if (inst->second_type == 2 || inst->second_type == 3) {
            inst->second_value = add_symbol_table(symbol_table, &symbol_count, 
                inst->second_value);
        }
    }
}

// ----------------------------------------------------------------------------

// Main function to decode the binary content then store all the instructions
// into the array of struct function_frame function_list
int decode_binary(char ** argv, struct function_frame * function_list) {
    FILE * fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        fprintf(stderr, "Binary file cannot be opened.\n");
        return 0;
    }
    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp)*8; // Find the number of bits present
    fseek(fp, 0, SEEK_SET);
    
    // Store an array of bits for decoding
    BIT * bitp = (BIT *)malloc(sizeof(BIT)*file_size);
    if (bitp == NULL) {
        fprintf(stderr, "Binary file is too large.\n");
        fclose(fp);
        return 0;
    }
    BYTE buffer;
    int index = 0;
    // Assign each index to a bit, using bitmasking on read bytes
    while (fread(&buffer, sizeof(BYTE), 1, fp) != 0) {
        for (int i = 0; i < 8; i++) {
            // Bit shifting by first applying a 1 mask to the target position,
            // then shifting it all the way to the right for assignment
            bitp[index] = ((1 << (7-i)) & buffer) >> (7-i);
            index += 1;
        }
    }
    fclose(fp);

    // ________________________________________________________________________

    int func_count = 0;
    int instruction_index = 1;
    index = file_size; 

    while (1) {
        struct function_frame func;

        // 5 bits for number of instruction
        func.instruction_count = convert_binary(bitp, index-5, 5);
        // Next 3 bits should always be RET
        if (convert_binary(bitp, index-5-3, 3) != 2) {
            fprintf(stderr, "RET is not found at the end of a function.\n");
            free(bitp);
            return 0;
        }

        instruction_index = 1; // INSTRUCTION SET
        index = index-5; // [Align indices to the end of the next field - opcode]
        for (int i = 0; i < func.instruction_count; i++) { 
            struct instruction instruct;
            instruct.operation_code = convert_binary(bitp, index-3, 3);         
            instruct.first_type = convert_binary(bitp, index-3-2, 2);

            // [MOV A B] (A cannot be a VALUE and B can be any type)
            if (instruct.operation_code == 0) { 
                if (instruct.first_type == 0) {
                    fprintf(stderr, "MOV A must be a non VALUE type.\n");
                    free(bitp);
                    return 0;
                }
                index = store_instruction(bitp, &instruct.first_type, 
                    &instruct.first_value, index-3-2);
                instruct.second_type = convert_binary(bitp, index-2, 2);
                index = store_instruction(bitp, &instruct.second_type, 
                    &instruct.second_value, index-2);
            
            // [CAL A] (A must be of type VALUE)
            } else if (instruct.operation_code == 1) { 
                if (instruct.first_type != 0) {
                    fprintf(stderr, "CAL operation call must be a VALUE type.\n");
                    free(bitp);
                    return 0;
                }
                index = store_instruction(bitp, &instruct.first_type, 
                    &instruct.first_value, index-3-2);
            
            // [RET] (No arguments are supplied)
            } else if (instruct.operation_code == 2) {
                index = index-3; // No values stored

            // [REF A B] (A cannot be a VALUE, B is a STACK SYMBOL or POINTER type)
            } else if (instruct.operation_code == 3) {
                if (instruct.first_type == 0) {
                    fprintf(stderr, "REF A cannot be a VALUE type.\n");
                    free(bitp);
                    return 0;
                }
                index = store_instruction(bitp, &instruct.first_type, 
                    &instruct.first_value, index-3-2);
                instruct.second_type = convert_binary(bitp, index-2, 2);
                // Either stack symbol or pointer types
                if (instruct.second_type != 2 && instruct.second_type != 3) {
                    fprintf(stderr, "REF B must be a STACK SYMBOL or POINTER VALUED type.\n");
                    free(bitp);
                    return 0;
                } 
                index = store_instruction(bitp, &instruct.second_type, 
                    &instruct.second_value, index-2);

            // [ADD A B] (Both must be REGISTER address types)
            } else if (instruct.operation_code == 4) {
                index = store_instruction(bitp, &instruct.first_type, 
                    &instruct.first_value, index-3-2);
                instruct.second_type = convert_binary(bitp, index-2, 2);
                index = store_instruction(bitp, &instruct.second_type, 
                    &instruct.second_value, index-2);
                if (instruct.first_type != 1 || instruct.second_type != 1) { 
                    fprintf(stderr, "ADD A B must be both REGISTER ADDRESS types.\n");
                    free(bitp);
                    return 0;
                }
            
            // [PRINT A] (A is any type)
            } else if (instruct.operation_code == 5) {
                index = store_instruction(bitp, &instruct.first_type, 
                    &instruct.first_value, index-3-2);
            
            // [NOT A] and [EQU A] (A is a REGISTER ADDRESS type)
            } else if (instruct.operation_code == 6 || instruct.operation_code == 7) {
                if (instruct.first_type != 1) {
                    if (instruct.operation_code == 6) {
                        fprintf(stderr, "NOT A must be a REGISTER ADDRESS type.\n");
                    } else {
                        fprintf(stderr, "EQU A must be a REGISTER ADDRESS type.\n");
                    }
                    free(bitp);
                    return 0;
                }
                index = store_instruction(bitp, &instruct.first_type, 
                    &instruct.first_value, index-3-2);
            }

            // Store the instructions in the function struct in a non-backward order
            func.instruction_list[func.instruction_count-instruction_index] = instruct;
            instruction_index += 1;
        }

        order_stack_addresses(&func);
        // Store the function label and frame into the function list
        func.label = convert_binary(bitp, index-3, 3);
        index = index-3;
        // If the function index exceeds 8, no more functions are added.
        // However func_count remains for the actual number of encoded functions
        if (func_count < 8) {
            function_list[func_count] = func; 
        }
        func_count += 1;
          
        // End the loop at Padding Bits (strictly less than 8)
        if (index-3 < 8) {
            break;
        }
    }

    int array_length = func_count;
    if (array_length > 8) {
        array_length = 8;
    }
    // Functions must be sorted back into correct order
    for (int i = 0; i < (int)(array_length/2)-1+1; i++) { // Reverse an array
        struct function_frame temp;
        temp = function_list[array_length-1-i];
        function_list[array_length-1-i] = function_list[i];
        function_list[i] = temp; 
    }

    free(bitp);
    return func_count;
}
