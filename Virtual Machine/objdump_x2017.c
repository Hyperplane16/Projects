#include "header_x2017.h"

// Prints the variable and type with a space preceeding it
// Note that stack symbols are already ordered in appearance, so printing is trivial
void print_instruction(BYTE * type, BYTE * value) {
    if (* type == 0) {
        printf(" VAL %d", * value);
    } else if (* type == 1) {
        printf(" REG %d", * value);
    } else if (* type == 2) {
        printf(" STK %c", * value+65);
    } else if (* type == 3) {
        printf(" PTR %c", * value+65);
    }
}

// Given a list of parsed functions, print out all instructions in assembly form
void disassembler_output(struct function_frame * function_list, int size) {
    // Stack symbols must be printed from A->Z->a->f (32 symbols) according to
    // the order of appearance of unique stack addresses, regardless of its value
    for (int i = 0; i < size; i++) {
        printf("FUNC LABEL %d\n", function_list[i].label);
        for (int j = 0; j < function_list[i].instruction_count; j++) {
            struct instruction inst = function_list[i].instruction_list[j];
            
            // MOV A B
            if (inst.operation_code == 0) { 
                printf("    MOV");
                print_instruction(&inst.first_type, &inst.first_value);
                print_instruction(&inst.second_type, &inst.second_value);
                printf("\n");
            // CAL A
            } else if (inst.operation_code == 1) { 
                printf("    CAL VAL %d\n", inst.first_value); // Only type is VALUE
            // RET
            } else if (inst.operation_code == 2) { 
                printf("    RET\n"); // No values
            // REF A B
            } else if (inst.operation_code == 3) {
                printf("    REF");
                print_instruction(&inst.first_type, &inst.first_value);
                print_instruction(&inst.second_type, &inst.second_value);
                printf("\n");
            // ADD A B
            } else if (inst.operation_code == 4) { 
                // Only type is REGISTER ADDRESS
                printf("    ADD REG %d REG %d\n", inst.first_value, inst.second_value);
            // PRINT A
            } else if (inst.operation_code == 5) { 
                printf("    PRINT");
                print_instruction(&inst.first_type, &inst.first_value);
                printf("\n");
            // NOT A
            } else if (inst.operation_code == 6) { 
                printf("    NOT");
                print_instruction(&inst.first_type, &inst.first_value);
                printf("\n");
             // EQU A
            } else if (inst.operation_code == 7) {
                printf("    EQU");
                print_instruction(&inst.first_type, &inst.first_value);
                printf("\n");
            }
        }
    }
}

int main(int argc, char ** argv) {
    struct function_frame function_list[8]; // Maximum 8 functions (3 bits) total
    int func_count = decode_binary(argv, function_list);
    if (func_count == 0) {
        return 1;
    } else if (func_count > 8) {
        fprintf(stderr, "More than 8 functions found: %d\n", func_count);
        return 1;
    }
    
    disassembler_output(function_list, func_count);
    return 0;
}