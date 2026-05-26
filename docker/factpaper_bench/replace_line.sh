#!/bin/bash

# Usage: ./replace_second_line.sh your_file.txt

file="$1"

if [ ! -f "$file" ]; then
  echo "File not found!"
  exit 1
fi

# Replace the second line with '\q'
sed '40s/.*/\\q/' "$file" > "${file}.tmp" && mv "${file}.tmp" "$file"

echo "40th line of '$file' replaced with \\q"
