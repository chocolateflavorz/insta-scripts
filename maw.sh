
#!/bin/bash

input_file=$1

num_lines=$2

delimiter=$(printf -- "- %.0s" $(seq 1 $num_lines))

paste -d ' ' $delimiter < $input_file 