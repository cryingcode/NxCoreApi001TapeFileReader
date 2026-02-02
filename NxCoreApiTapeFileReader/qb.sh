#!/usr/bin/env bash

# file: qb.sh   (quick build script)

rm -rf build && mkdir build
cmake -S . -B build -G Ninja -DCMAKE_VERBOSE_MAKEFILE=ON
cmake --build build --parallel
sudo rm -rf /usr/local/bin/nxcore-tape-reader /usr/local/lib/libtapefilereaderproject.a /usr/local/lib/extern/nxcore
sudo cmake --install build 
sudo ldconfig