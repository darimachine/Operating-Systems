#!/bin/bash

# Loop through all .txt files in the directory
for file in *.txt; do
  # Extract the filename without the extension
  base_name="${file%.txt}"

  # Rename the file with the new .sh extension
  mv "$file" "$base_name.sh"
done

echo "All .txt files have been renamed to .sh files."
