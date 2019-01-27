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
git submodule update --init --recursive
mkdir build
cd build
cmake ..
make
```

After build, `ejudge` executable file will place at `build/src/ejudge`

## Run tests

### Requirements

* Prepare a VM runtime file
  * Download latest [aleth release](https://github.com/ethereum/aleth/releases).
  * After unziping the archive, you can find the VM runtime in `lib/libaleth-interpreter.so`.

* (option) Solidity compiler `solc`

### Basic usage

`./ejudge <TESTCASES_PATH> <VMFILE_PATH>`

### Test with solidity source

set environment variable `SOLIDITYC` to where the `solc` at.

```bash
export SOLIDITYC="<Your/Solc/Path>"

./ejudge <TESTCASES_PATH> <VMFILE_PATH>
# For example, ./ejudge ./testcase/ ./bin/libaleth-interpreter.so
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
