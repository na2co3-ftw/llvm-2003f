# llvm-2003f

The LLVM backend implementation for [2003'd ferlesyl](http://jurliyuuri.com/OS/) the fictional architecture.

## Build on Linux

Install prerequisites:
```
sudo apt install clang-5.0 llvm-5.0 cmake make
```

Build:
```
./build.sh
```
generates ./build/bin/llc

## Build on Windows

Install prerequisites:
- Miscrosoft Visual Studio 2017
- CMake >= 3.4.3
- Python 2.7.x

Build:
1. create `build` Directory in the root of this repository
1. open Command Prompt at `build`
1. run `cmake -DLLVM_TARGETS_TO_BUILD="F2003f" ../src`
    - or `cmake -DLLVM_TARGETS_TO_BUILD="F2003f" -Thost=x64 ../src` (in 64-bit environment)
1. run `cmake --build . --target llc`

generates ./build/Debug/bin/llc.exe


## Usage

Example:
```
./build/bin/llc sample/test1.ll -march=2003f
```

## Directory Structure

[src/lib/Target/F2003f](src/lib/Target/F2003f/) is the 2003f backend implementation body.

Some other files are also modified to support it.
