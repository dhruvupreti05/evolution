#!/bin/bash

OUTPUT_FILE=".srcout.txt"

> "$OUTPUT_FILE"

REPO=$(git rev-parse --show-toplevel)
cd "$REPO"

if [ ! -d "src" ]; then
    echo "Error: 'src' directory not found in the current folder."
    exit 1
fi

find src -type f \( -name "*.cpp" -o -name "*.h" \) | while read -r file; do
    echo "//  [$file] --------------------------------------------------------------" >> "$OUTPUT_FILE"
    
    cat "$file" >> "$OUTPUT_FILE"
    
    echo -e "\n\n" >> "$OUTPUT_FILE"
done

echo "Done! All source files have been combined into $OUTPUT_FILE."