#!/usr/bin/env bash
set -euo pipefail
cd /workspace/TeiemBeats
export XDG_CACHE_HOME=/workspace/.cache
mkdir -p "$XDG_CACHE_HOME"
export PKG_CONFIG_SYSROOT_DIR=/workspace/sysroot
export PKG_CONFIG_LIBDIR=/workspace/sysroot/usr/lib/x86_64-linux-gnu/pkgconfig:/workspace/sysroot/usr/share/pkgconfig:/usr/lib/x86_64-linux-gnu/pkgconfig:/usr/share/pkgconfig
export CPLUS_INCLUDE_PATH=/workspace/sysroot/usr/include:/workspace/sysroot/usr/include/freetype2
export C_INCLUDE_PATH=/workspace/sysroot/usr/include:/workspace/sysroot/usr/include/freetype2
export LIBRARY_PATH=/workspace/sysroot/usr/lib/x86_64-linux-gnu
CMAKE=/workspace/tools/cmake/data/bin/cmake
"$CMAKE" -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/workspace/sysroot/usr -DCMAKE_LIBRARY_PATH=/workspace/sysroot/usr/lib/x86_64-linux-gnu -DCMAKE_INCLUDE_PATH=/workspace/sysroot/usr/include
"$CMAKE" --build build --parallel 4
/workspace/tools/cmake/data/bin/ctest --test-dir build --output-on-failure
