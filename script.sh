#!/bin/bash
make all
test_folder_name="tests2/tests"
command="./hw3" #change this to your name of file, keep the ''./''
for ((i=1; i<=58; i++))
do
    filename="$test_folder_name/t${i}.in"
    filename_our_out="$test_folder_name/t${i}.our.out"
    filename_out="$test_folder_name/t${i}.out"
    if [ -f "$filename" ]; then
	    "$command" < "$filename" >"$filename_our_out"
        echo "diffing test no.${i}"
        diff "$filename_out" "$filename_our_out"
       

    else
        echo "File not found: $filename"
    fi
done

