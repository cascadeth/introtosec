#!/bin/bash

# Convert input file parameter into a postscript hex dump

# xdd adds new lines every so often in the dump,
# so pull delete new lines out of the file

binary_file_content=$(xxd -p $1 | tr -d "\n")



# Grab the original file name to use for output file name
original=$(echo $1 | cut -d "." -f 1)


bitmap_header='424d6af64700000000008a0000007c000000e80300009b0400000100200003000000e0f5470000000000000000000000000000000000' # The bitmap file header
binary_data=$(echo ${binary_file_content:107})	# Pull the substring after the 108th(54 * 2) position (starts at 0, so 108 - 1 => 107) in the "string"

printf "$bitmap_header" > temp		# Print the postscript hex bitmap file header to a temp file
printf "$binary_data" >> temp		# Print the postscript binary data to the temp file

xxd -r -p temp > "$original".bmp	# Reverse convert the file back to raw binary data

rm temp					# Remove the temp raw hex bitmap file
