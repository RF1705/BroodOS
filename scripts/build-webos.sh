#!/bin/sh
set -eu

if [ -z "${WEBOS_NDK_ROOT:-}" ]; then
    echo "WEBOS_NDK_ROOT must point to arm-webos-linux-gnueabi_sdk-buildroot" >&2
    exit 1
fi

toolchain_file="$WEBOS_NDK_ROOT/share/buildroot/toolchainfile.cmake"
if [ ! -f "$toolchain_file" ]; then
    echo "webOS toolchain file not found: $toolchain_file" >&2
    exit 1
fi

cmake -S . -B build-webos \
    -DCMAKE_BUILD_TYPE=Release \
    -DBROODOS_BUILD_TESTS=OFF \
    -DCMAKE_TOOLCHAIN_FILE="$toolchain_file"
cmake --build build-webos --parallel

if command -v ares-package >/dev/null 2>&1; then
    cmake --build build-webos --target package
else
    echo "ARM binary built. Install @webos-tools/cli to create an IPK." >&2
fi

