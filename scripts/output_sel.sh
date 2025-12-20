#!/bin/bash
#
# opt_select_ncurses - Shell utilities for ncurses-based selector
#
# Copyright (c) 2025, Shyju N
# Email: n.shyju@gmail.com
#
# Licensed under the BSD 3-Clause License.
# See the LICENSE file in the project root for full license information.
#


#set -x

# Generate a random temp_filename
temp_filename=$(mktemp)
echo "random file name: $temp_filename"
echo

echo

# Write a predefined string to the file
echo "Data from pipe" > $temp_filename

# Read from standard input and append to the file
while IFS= read -r line; do
    echo "$line" >> $temp_filename
done

opt_select_ncurses $temp_filename $temp_filename piped_input=yes

return_value=$?
# check if opt_select_ncurses returned non-zero
if [ $return_value -ne 0 ]; then
    echo "opt_select_ncurses failed with exit code: $return_value"
    rm -rf $temp_filename
    exit
fi
actual_file=`cat $temp_filename`
rm -rf $temp_filename

echo "$0: file: $actual_file"
