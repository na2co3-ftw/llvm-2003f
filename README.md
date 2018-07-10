# llvm-2003f

The LLVM backend implementation for [2003'd ferlesyl](http://jurliyuuri.com/OS/) the fictional architecture.

## Build

Install prerequisites:
```
sudo apt install clang-5.0 llvm-5.0 cmake make
```

Build:
```
./build.sh
```
generates ./build/bin/llc

## Usage

Example:
```
./build/bin/llc sample/test1.ll -march=2003f
```

## Directory Structure

[src/lib/Target/F2003f](src/lib/Target/F2003f/) is the 2003f backend implementation body.

Some other files are also modified to support it.
