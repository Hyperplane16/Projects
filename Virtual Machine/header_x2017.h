#ifndef header_x2017
#define header_x2017

#include <stdio.h>
#include <stdlib.h>

#define BYTE unsigned char
#define BIT unsigned char

struct instruction {
    BYTE operation_code;
    BYTE first_type;
    BYTE first_value;
    BYTE second_type;
    BYTE second_value;   
};

struct function_frame {
    BYTE label;
    BYTE instruction_count;
    struct instruction instruction_list[32];
    // Maximum 32 instructions (5 bits) per function
};

extern int decode_binary(char ** argv, struct function_frame * function_list);

#endif