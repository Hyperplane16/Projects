#!/bin/bash

count=0
for file in `ls personal_tests | grep ".c$"`; do
    # Filename (shell parameter expansion)
    file="${file%.*}" # Removes file extension

    in_file=personal_tests/$file.c
    out_file=personal_tests/$file.out

    gcc $in_file -o personal_tests/$file -L"." -lbtreestore -g -fsanitize=address -ftest-coverage -fprofile-arcs
    ./personal_tests/$file 2>&1 | diff --ignore-all-space - $out_file &>/dev/null && 
        echo "$file PASSED" || echo "$file <<FAILED>>"
    
    # Move gcov files into the test directory
    # (if they are not moved from the main area an error message arises at execution, although it is unaffected)
    mv $file.gcda personal_tests/coverage
    mv $file.gcno personal_tests/coverage

    count=$((count+1)) 
    rm personal_tests/$file 
done

printf "\nSuccessfully ran $count tests!\n"
