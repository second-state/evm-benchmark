# ejudge 1.0.0

Ejudge provide a benchmark framework for Ethereum Virtual Machine implementation


## Support OS

* Ubuntu >= 16.04

## Build Reqirement

C++ 17 Support Compiler
 - Filesystem

* g++ >= 8.1
* cmake >= 3.8

## How to build

```bash
git clone https://xxxx/benchmarks.git
cd benchmarks
mkdir build
cd build
cmake ..
make
```

After build, `ejudge` executable file will place at `build/src/ejudge`

## Run tests

* Basic usage

`./ejudge TESTCASES_PATH VMFILE`

* Test with solidity source

set environment variable `SOLDITYC` to where the `solc` at.

```bash
export SOLDITYC="/mnt/d/User/Documents/Git/EthBrenchmark/bin/solc-static-linux" 

./ejudge ./testcase/ ./bin/libaleth-interpreter.so
 
```

## Todos

### Build system

- [ ] Windows MSVC
- [ ] Windows GCC (Can not fix on gcc 8.1)
- [x] Ubuntu 16 GCC
- [ ] Ubuntu 16 Clang
- [ ] MacOS GCC
- [ ] MacOS Clang

### Filesystem

- [ ] implement ~~~`recursive_directory_iterator`~~~ `filesystem` for old compiler
