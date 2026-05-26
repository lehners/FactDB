#!/bin/bash

set -euo pipefail

# Clone repo only if missing
if [ ! -d "LMFAO" ]; then
  git clone https://github.com/fdbresearch/LMFAO.git
else
  echo "LMFAO repo already exists, skipping clone."
fi

cd LMFAO

# Configure only if not already configured
if [ ! -f "Makefile" ] && [ ! -d "CMakeFiles" ]; then
  cmake .
else
  echo "CMake already configured, skipping."
fi

# Build only if binary not present
# (adjust 'lmfao' if the actual binary has a different name)
make -j"$(nproc)"