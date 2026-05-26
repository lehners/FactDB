#!/usr/bin/env bash

set -euo pipefail

echo "Set CE benchmark up..."

mkdir -p bench/data/ce

cd bench/data/ce
echo '2c3aaaa766930aee95e8c5d8e0b10c4526f960c585414f873a31070ee3049add  cebench.tar.zst' | sha256sum --check --status 2>/dev/null || curl -OL https://db.in.tum.de/~birler/dbgen/cebench.tar.zst
echo '2c3aaaa766930aee95e8c5d8e0b10c4526f960c585414f873a31070ee3049add  cebench.tar.zst' | sha256sum --check --status
tar --skip-old-files -xf cebench.tar.zst

echo "CE setup done."