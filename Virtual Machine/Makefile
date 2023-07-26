CC=gcc
CFLAGS=-fsanitize=address -Wvla -Wall -Werror -g -std=gnu11 -lasan

vm_x2017: vm_x2017.c
	$(CC) $(CFLAGS) disassembler_x2017.c $^ -o $@

objdump_x2017: objdump_x2017.c
	$(CC) $(CFLAGS) disassembler_x2017.c $^ -o $@

tests:
	echo "Setup has already been completed"

run_tests: export ASAN_OPTIONS=verify_asan_link_order=0
run_tests:
	$(CC) $(CFLAGS) disassembler_x2017.c vm_x2017.c -o vm_x2017
	$(CC) $(CFLAGS) disassembler_x2017.c objdump_x2017.c -o objdump_x2017
	bash test.sh

clean:
	rm objdump_x2017
	rm vm_x2017
