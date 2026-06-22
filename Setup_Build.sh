#!/bin/sh

cmake -DCMAKE_BUILD_TYPE=Debug "-DCMAKE_MAKE_PROGRAM=C:/Program Files/JetBrains/CLion 2025.3.1
.1/bin/ninja/win/x64/ninja.exe" -G Ninja -S . -B build

