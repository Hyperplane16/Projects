#!/bin/bash

count=0
for file in `ls personal_tests | grep ".c$"`; do
    # Filename (shell parameter expansion)
    file="${file%.*}" # Removes file extension

    in_file=personal_tests/$file.c
    out_file=personal_tests/$file.out

    gcc $in_file virtual_alloc.c -o personal_tests/$file
    ./personal_tests/$file 2>&1 | diff --ignore-all-space - $out_file &>/dev/null && 
        echo "$file PASSED" || echo "$file <<FAILED>>"

    count=$((count+1))
    rm personal_tests/$file 
done

printf "\nSuccessfully ran $count tests!\n"
