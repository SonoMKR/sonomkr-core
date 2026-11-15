#!/usr/bin/env bash
# Configure, build and run the whole test suite.
#   tests/run-tests.sh [build directory] [extra ctest arguments...]
set -euo pipefail

SRC_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${1:-$SRC_DIR/build}"
shift || true

cmake -S "$SRC_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" -j"$(nproc)"
ctest --test-dir "$BUILD_DIR" --output-on-failure "$@"
