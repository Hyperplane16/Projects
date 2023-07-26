./procchat &>/dev/null & 
server_pid=$!

count=0
for file in `ls personal_tests | grep ".c$"`; do
    # Filename (shell parameter expansion)
    file="${file%.*}" # Removes file extension

    in_file=personal_tests/$file.c
    out_file=personal_tests/$file.out

    gcc $in_file -o personal_tests/$file
    ./personal_tests/$file
    if [ $? == 0 ] 
    then
        echo "$file PASSED"
    else
        echo "$file FAILED"
    fi

    count=$((count+1))
    rm personal_tests/$file 
done

printf "\nSuccessfully ran $count tests!\n"

# Clean up testing directories
kill $(ps -aux | grep './procchat' | awk '{print$2}') &>/dev/null
rm -r test_folder
