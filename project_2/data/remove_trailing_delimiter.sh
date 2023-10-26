#!/bin/bash

# Get the directory where the script is located
directory="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Loop through each .tbl file and remove trailing "|"
for file in "$directory"/*.tbl; do
    if [ -f "$file" ]; then
        sed -i 's/|*$//' "$file"
        echo "Removed trailing '|' from $(basename "$file")"
    fi
done

