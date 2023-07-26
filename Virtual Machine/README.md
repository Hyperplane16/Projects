# Virtual Machine

An implementation of a virtual machine in C to emulate instructions written in a 
custom assembly language named x2017. An 8 bit architecture is adopted consisting of
RAM with 2^8 addresses, 8 registers and program code.

disassembler_x2017.c accepts, disassembles x2017 binaries and stores all 
instructions in program code. objdump_x2017.c dumps these instructions back
into human-readable assembly. vm_x2017.c emulates the execution of the 
disassembled program code through a stack data structure.

x2017 supports many operations including moving values between registers (MOV),
calling and returning from functions (CAL and RET), storing stack pointer addresses (REF), 
adding values (ADD), printing output (PRINT) and the NOT, EQU bit operations.

## Testing

A comprehensive testing suite is created with an activation script test.sh to test
several x2017 programs with interesting use cases.