#!/bin/bash

set -euo pipefail

# Clone the FBENCH repository (only if missing)
if [ ! -d "FBENCH" ]; then
  git clone https://github.com/fdbresearch/FBENCH.git
else
  echo "FBENCH repo already exists, skipping clone."
fi

cd FBENCH

# Apply patch only if not already applied
if git apply --reverse --check ../scripts/bench/fbench.patch >/dev/null 2>&1; then
  echo "Patch already applied, skipping."
else
  git apply ../scripts/bench/fbench.patch
fi

# Build only if binary does not exist
if [ ! -f "build/fbench" ]; then
  mkdir -p build
  cd build
  cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5

  make -j"$(nproc)"
  cd ..
else
  echo "Binary already built, skipping build."
fi

cd ..

# Create symlink only if missing or broken
if [ ! -L "fbench" ]; then
  ln -s "$(pwd)/FBENCH/build/fbench" fbench
elif [ ! -e "fbench" ]; then
  # broken symlink → recreate
  rm fbench
  ln -s "$(pwd)/FBENCH/build/fbench" fbench
else
  echo "Symlink already exists, skipping."
fi