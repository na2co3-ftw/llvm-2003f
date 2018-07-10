#!/usr/bin/env bash

mkdir -p build
cd build
cmake -DCMAKE_CXX_COMPILER=clang++-5.0 -DLLVM_TARGETS_TO_BUILD="F2003f" -DLLVM_TABLEGEN="/usr/bin/llvm-tblgen-5.0" ../src
make -j6 -l6 llc
