#!/bin/bash

# Exit on any error
set -e

cd "$(git rev-parse --show-toplevel)"

# Check if a scale factor is provided as a command-line argument
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <scale_factor>"
    exit 1
fi

# Assign the scale factor from the command-line argument
scale_factor="$1"

# Local destination folder
local_folder="data/"

mkdir -p "${local_folder}"

# Copy the file using scp
scp "hyper2:/home/lehner/data/bi-sf${scale_factor}-composite-merged-fk/data/Person_knows_Person.csv" "$local_folder"
scp "hyper2:/home/lehner/data/bi-sf${scale_factor}-composite-merged-fk/data/Person.csv" "$local_folder"
