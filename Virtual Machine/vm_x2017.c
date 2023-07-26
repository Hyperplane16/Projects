#include "header_x2017.h"

// Starting execution from the entrypoint function of label 0,
// when a function is called, its return address (address of next instruction)
// will be pushed into the stack (0 is a custom/dummy code for label 0).
// Execution is line-by-line with local variables pushed in one line at a time.
// The top of the stack will always be a value storing the current number of
// local variables on the current stack frame. (Adding new variables will cause
// the value to be popped, variables added, and the value re-pushed to the top)
// When the function returns, all local variables will be popped and the PC
// will point to the return address popped from the stack frame.

// The functions related to stack operations as well as handling/testing operations
// of the instructions all are able to suspend the execution of the VM using exit(1).
// This is justified by the error checking that would be too numerous to 
// perform or pass back to the main function.

// ____________________________________________________________________________

// Push a BYTE of data to the top of the stack (note relevant data starts at 1)
void push_stack(BYTE data, BYTE * stack, BYTE * reg) {
    reg[4] += 1;
    if (reg[4] >= 255) { // Stack Overflow
        fprintf(stderr, "The stack has overflowed in memory.\n");
        exit(1);
    }
    stack[reg[4]] = data;
}

// Removes a BYTE of data from the top of the stack and returns
BYTE pop_stack(BYTE * stack, BYTE * reg) {
    if (reg[4] <= 0) {
        fprintf(stderr, "Illegal access within the stack.\n");
        exit(1);
    }
    reg[4] -= 1;
    return stack[reg[4]+1];
}

// Return the value in the stack from the SP with an offset downwards
BYTE peek_stack(int offset, BYTE * stack, BYTE * reg) {
    if (reg[4]-offset <= 0 || offset < 0) {
        fprintf(stderr, "Illegal access within the stack.\n");
        exit(1);
    }
    return stack[reg[4]-offset];
}

// Given a function label, attempt to set the PC (reg 7) to point to its first
// instruction in program memory.
// Returns the number of functions with the specified label found
int locate_label(int label, BYTE * label_map, BYTE * reg) {
    int found = 0;
    for (int i = 0; i < reg[5]; i++) {
        if (label_map[i] == label) {
            // Set PC to the starting index of the function's first instruction
            // (Each instruction set is 32 long)
            reg[7] = (32 * i); 
            found += 1;
        }
    }
    return found;
}

// Attempts to allocate a stack space with a value specified by reg[6]
void allocate_stack_variable(BYTE value, BYTE curr_varc, BYTE * stack, BYTE * reg) {
    if (curr_varc <= value) {
        // If variable has not been allocated, push to the stack to allocate it
        pop_stack(stack, reg);
        push_stack(reg[6], stack, reg);
        push_stack(curr_varc+1, stack, reg); // Reset varc back to the top
    } else { // Otherwise, edit the stack space of the value
        stack[reg[4]-(curr_varc-value)] = reg[6];
    }
}

// Detects whether the stack symbol has already been allocated given the type:
// [0]: STACK SYMBOL
// [1]: POINTER VALUED (this only occurs with accessing a symbol using PTR)
void test_stack_variables(BYTE value, BYTE curr_varc, int type) {
    if (curr_varc <= value) {
        if (type == 0) {
            // If the current number of stored arguments is not larger than the 
            // stack symbol address we need, it has not yet been allocated
            fprintf(stderr, "Stack symbol %c has not been initiated.\n", value+65);
        } else if (type == 1) {
            // If the pointer is not allocated yet, its value cannot be accessed
            fprintf(stderr, "Trying to access a pointer that does not exist.\n");
        }
        exit(1);
    } 
}

// Handles register_operations given a mode:
// [0]: Retreival of data from a register with a given value (stored in reg[6])
// [1]: Storage of data into a register with a given value (from reg[6])
void handle_register_operations(BYTE value, int mode, BYTE opcode, BYTE * reg) {
    // Programs cannot access personal and illegal registers
    if (value == 4 || value == 5 || value == 6 || value >= 8) {
        fprintf(stderr, "Register %d cannot be accessed by instructions.\n", value);
        exit(1);
    } 
    else if (mode == 0) { // MOVE TO 6
        reg[6] = reg[value];
    } else if (mode == 1) { // MOVE FROM 6
        reg[value] = reg[6];
    }
}

// Handles cases when a value from reg[6] is moved to the PC, reg[7]
void handle_pc_changes(BYTE opcode, struct instruction * program_memory,
    BYTE * label_map, BYTE * count_map, BYTE * reg, BYTE * stack) {
    // When editing PC, the instruction number moved in must be relative to the
    // start of the current function. The address in memory of its first 
    // instruction must be added as an offset. Additionally the PC cannot be set
    // to an index greater than the number of instructions in the current function.
    //   - When using ADD (opcode 4) the former process is not performed
    // Note that the PC still increments after changes are added to it

    // Return address of the instruction AFTER the CAL of the current function
    BYTE address = peek_stack(peek_stack(0, stack, reg)+1, stack, reg);
    if (address == 0) { // If main, there is no call operation before it
        locate_label(0, label_map, reg);
    } else {
        // Retreive label of the CAL operation then use locate_label to set PC
        locate_label(program_memory[address-1].first_value, label_map, reg);
    }  
    // - Add the number of instructions (from count_map) to the relative PC
    // - Dividing the PC by 32 and rounding down retreives its function index
    BYTE max_inst_index = count_map[(reg[7]/32)] + reg[7];
    if (opcode == 4) { // ADD OPERATION (within the main adding has already occured)
        reg[7] = reg[6];
    } else { // All other operations
        reg[7] += reg[6];
    }
    // The PC cannot exceed the number of instructions in the function minus 1
    // (the PC increments it again afterward)
    if (reg[7] >= max_inst_index-1) { 
        fprintf(stderr, "PC references non-existent instruction in the function scope.\n");
        exit(1);
    }
}

// Parse the list of function_frames into a list of instructions, and store 
// function related data in other arrays
int parse_program_memory(char **argv, struct instruction * program_memory, 
    BYTE * label_map, BYTE * count_map, BYTE * reg) {
    struct function_frame function_list[8]; // Maximum of 8 functions (3 bits) total
    // Reg[5] stores the number of functions
    reg[5] = decode_binary(argv, function_list); 
    if (reg[5] == 0) {
        return 0;
    } else if (reg[5] > 8) {
        fprintf(stderr, "More than 8 functions found: %d\n", reg[5]);
        return 0;
    }

    // Store instructions consecutively for program memory access 
    // (PC has max size of 256 so this is necessary)
    int index = 0;
    for (int i = 0; i < reg[5]; i++) {
        for (int j = 0; j < 32; j++) {
            // Store the instruction sets
            program_memory[index] = function_list[i].instruction_list[j];
            index += 1;
        }
        label_map[i] = function_list[i].label; // LABELS
        count_map[i] = function_list[i].instruction_count; // INSTRUCTION COUNT
    }
    return 1;
}

// ____________________________________________________________________________

int main(int argc, char **argv) {
    // PROGRAM MEMORY
    // Fields stores 8 instructions sets of 32 consecutively
    struct instruction program_memory[8*32]; 
    BYTE label_map[8]; // Maps a set of instructions at an index with its LABEL
    BYTE count_map[8]; // Maps the number of instructions
    // (e.g. third index will map the label+count of the third instruction set)

    // REGISTER BANK (0->3: Program Access, 4->6: Personal, 7: Program Counter)
    BYTE reg[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // Data initialised to 0
    // Reg[4]: Stores the STACK POINTER
    // Reg[5]: Stores the number of functions in total
    // Reg[6]: Stores temporary values for each instruction, if applicable
    // Reg[7]: Program Counter: Stores the instruction number within a function

    // RAM STACK (note that the addresses go from lowest to highest)
    BYTE stack[256];

    if (parse_program_memory(argv, program_memory, label_map, count_map, reg) == 0) {
        return 1;
    }

    // Find the function with label 0 as the entry point
    int label_count = locate_label(0, label_map, reg);
    if (label_count != 1) {
        fprintf(stderr, "%d functions of label 0 found.\n", label_count);
        return 1;
    }

    push_stack(0, stack, reg); // Custom return address of function label 0
    push_stack(0, stack, reg); // Number of local variables currently stored
    while (1) {
        // Current number of local variables in the function
        BYTE curr_varc = stack[reg[4]]; 
        BYTE opcode = program_memory[reg[7]].operation_code;
        BYTE second_type = program_memory[reg[7]].second_type;
        BYTE second_value = program_memory[reg[7]].second_value;
        BYTE first_type = program_memory[reg[7]].first_type;
        BYTE first_value = program_memory[reg[7]].first_value;
        
        // [MOV A B] (A cannot be a VALUE and B can be any type)
        if (opcode == 0) {
            if (second_type == 0) { // VALUE
                reg[6] = second_value;
            } else if (second_type == 1) { // REGISTER
                handle_register_operations(second_value, 0, opcode, reg);
            } else if (second_type == 2) { // STACK SYMBOL
                test_stack_variables(second_value, curr_varc, 0);
                // Obtain the value of the stack space allocated for the symbol
                reg[6] = peek_stack(curr_varc-second_value, stack, reg); 
            } else if (second_type == 3) { // POINTER VALUED
                test_stack_variables(second_value, curr_varc, 0);
                // Obtain the value stored at the global address specified by
                // the stack space in the stack
                reg[6] = stack[peek_stack(curr_varc-second_value, stack, reg)];
            }

            if (first_type == 1) { // REGISTER
                if (first_value == 7) {
                    handle_pc_changes(opcode, program_memory, label_map, 
                        count_map, reg, stack);
                } else {
                    handle_register_operations(first_value, 1, opcode, reg);
                }
            } else if (first_type == 2) { // STACK SYMBOL
                allocate_stack_variable(first_value, curr_varc, stack, reg);
            } else if (first_type == 3) { // POINTER VALUED
                test_stack_variables(first_value, curr_varc, 1);
                // Edit the stack at a global address specified by the stack space
                stack[peek_stack(curr_varc-first_value, stack, reg)] = reg[6];
            }

        // [CAL A] (A must be of type VALUE)
        } else if (opcode == 1) {
            // Opcode of the next instruction after function call
            push_stack(reg[7]+1, stack, reg); 
            push_stack(0, stack, reg); // No local variables at function start
            int label_count = locate_label(first_value, label_map, reg);
            if (label_count != 1) {
                fprintf(stderr, "Illegal count of %d functions for label %d found.\n",
                    label_count, first_value);
                return 1;
            }
            continue;
            
        // [RET] (No arguments are supplied)
        } else if (opcode == 2) {
            pop_stack(stack, reg);
            for (int i = 0; i < curr_varc; i++) { // Pop all the local variables
                pop_stack(stack, reg);
            }
            reg[7] = pop_stack(stack, reg); // Retreive the return address
            // If stack is empty, this is the final return from function label 0
            if (reg[4] == 0) { 
                break; 
            }
            continue;
            
        // [REF A B] (A cannot be a VALUE, B is a STACK SYMBOL or POINTER type)
        } else if (opcode == 3) {
            test_stack_variables(second_value, curr_varc, 0);
            if (second_type == 2) { // STACK SYMBOL
                // Obtain the global stack address (in relation to stack pointer)
                reg[6] = reg[4]-(curr_varc-second_value);
            } else if (second_type == 3) { // POINTER VALUED
                // Use the value stored in stack space and as the REF address
                reg[6] = peek_stack(curr_varc-second_value, stack, reg); 
            }


            if (first_type == 1) { // REGISTER
                if (first_value == 7) {
                    handle_pc_changes(opcode, program_memory, label_map, 
                        count_map, reg, stack);
                } else {
                    handle_register_operations(first_value, 1, opcode, reg);
                }
            } else if (first_type == 2) { // STACK SYMBOL
                allocate_stack_variable(first_value, curr_varc, stack, reg);
            } else if (first_type == 3) { // POINTER VALUED
                test_stack_variables(first_value, curr_varc, 1);
                // Edit the stack at a global address specified by the stack space
                stack[peek_stack(curr_varc-first_value, stack, reg)] = reg[6];
            } 
        
        // [ADD A B] (Both must be REGISTER address types)
        } else if (opcode == 4) {
            // If addition causes the value to overflow (>255), it will loop around
            // to 0 (value-256). This is defined behaviour for unsigned chars (BYTE) 
            handle_register_operations(second_value, 0, opcode, reg);
            if (first_value == 7) {
                // Special prevent test for adding of PC
                int total = (int)reg[7] + (int)reg[6];
                if (total > 255) {
                    fprintf(stderr, "PC references non-existent instruction in program memory.\n");
                    return 1;
                }
            }
            reg[6] += reg[first_value];
            if (first_value == 7) {
                handle_pc_changes(opcode, program_memory, label_map, 
                    count_map, reg, stack);
            } else {
                handle_register_operations(first_value, 1, opcode, reg);
            }
    

        // [PRINT A] (A is any type)
        } else if (opcode == 5) {
            if (first_type == 0) { // VALUE
                printf("%u\n", first_value);
            } else if (first_type == 1) { // REGISTER
                handle_register_operations(first_value, 0, opcode, reg);
                printf("%u\n", reg[6]);
            } else if (first_type == 2) { // STACK SYMBOL
                test_stack_variables(first_value, curr_varc, 0);
                // Obtain the value of the stack space allocated for the symbol value
                printf("%u\n", peek_stack(curr_varc-first_value, stack, reg)); 
            } else if (first_type == 3) { // POINTER VALUED
                test_stack_variables(first_value, curr_varc, 0);
                // Retreive the value of the stack at the global address specified
                printf("%u\n", stack[peek_stack(curr_varc-first_value, stack, reg)]); 
            }
        
        // [NOT A] (A is a REGISTER ADDRESS type)
        } else if (opcode == 6) {
            handle_register_operations(first_value, 0, opcode, reg);
            reg[6] = ~reg[6];
            handle_register_operations(first_value, 1, opcode, reg);
        
        // [EQU A] (A is a REGISTER ADDRESS type)
        } else if (opcode == 7) {
            handle_register_operations(first_value, 0, opcode, reg);
            if (reg[6] == 0) {
                reg[6] = 1;
            } else {
                reg[6] = 0;
            }
            handle_register_operations(first_value, 1, opcode, reg);
        } 

        // Increment PC: Next instruction to be accessed
        reg[7] = reg[7] + 1;
    }
}
