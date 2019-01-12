## Support OS

* Ubuntu >= 16.04
* (X) MacOS 
    * Linker issue 
* Windows 10
    * (X) GCC Because of g++8.1 compiler bug, it can not build. 
    * (X) MSVC 2017 (Need to Fix)

## Build Reqirement

C++ 17 Support
 - Filesystem

* g++ >= 8.1
* MSVC 2017
* cmake >= 3.8 ( maybe too high? )


# Check point

## Build system

- [ ] Windows MSVC
- [ ] Windows GCC (Can not fix on gcc 8.1)
- [x] Ubuntu 16 GCC
- [ ] Ubuntu 16 Clang
- [ ] MacOS GCC
- [ ] MacOS Clang

## Filesystem

- [x] Study Filesystem in C++17
- [ ] implement ~~~`recursive_directory_iterator`~~~ `filesystem` for old compiler

## Testcase Builder

- [x] Framework
- [x] Binary
- [x] Solidty
- [x] Lity

* some problem to solve (Done)
    * how to execute Makefile 
    * how to check execute successfully or not


## CEVM

- [ ] collection the librarys to tests
    - [x] cpp-eth (aleth-interpreter)
    - [ ] Hera
    - [ ] Evm JIT
- [ ] Test Connect code with OPCODE `timestamp`