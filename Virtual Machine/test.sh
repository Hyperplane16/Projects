#!/bin/bash
# Use xxd -p filename to hexdump binary to standard output

count=0
for file in `ls tests | grep ".x2017$"`; do
    # Filename (shell parameter expansion)
    file="${file%.*}" # Removes file extension

    binary_file=tests/$file.x2017
    print_file=tests/$file.asm
    out_file=tests/$file.out
    
    # dos2unix $binary_file
    # dos2unix $print_file
    # dos2unix $out_file

    ./objdump_x2017 $binary_file 2>&1 | diff - $print_file &>/dev/null && 
        echo "$file objdump PASSED" || echo "$file objdump <<FAILED>>"

    # Add "\n$?" to the end of vm output, where $? is the exit code
    printf "$(./vm_x2017 $binary_file 2>&1)\n$?" | diff - $out_file &>/dev/null && 
        echo "$file vm PASSED" || echo "$file vm <<FAILED>>"

    count=$((count+1)) 
done

printf "\nSuccessfully ran $count tests!\n"
