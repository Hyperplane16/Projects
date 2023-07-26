#! /usr/bin/env sh
echo "##########################"
echo "### Running e2e tests! ###"
echo "##########################\n"
count=0 # number of test cases run so far

# Assume all `.in` and `.out` files are located in a separate `tests_e2e` directory

for folder in `ls -d tests_e2e/*/ | sort -V`; do
    name=$(basename "$folder")
    
    echo Running test $name.

    config_file=tests_e2e/$name/$name.config
    expected_file=tests_e2e/$name/$name.out
    in_file=tests_e2e/$name/$name.in

    # dos2unix $config_file
    # dos2unix $expected_file
    # dos2unix $in_file
    
    python3 run.py $config_file null print_input < $in_file | diff - $expected_file || echo "Test $name: failed!\n"

    count=$((count+1))
done

echo "Finished running $count tests!"