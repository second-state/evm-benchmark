# ejudge 1.0.0

Ejudge provide a benchmark framework for Ethereum Virtual Machine implementation


## Support OS

* Ubuntu >= 16.04

## Build Reqirement

C++ 17 Support Compiler
 - Filesystem

* g++ >= 8.1
* cmake >= 3.8

## Get pre-built docker image

```bash
docker pull hydai/ejudge-dev
docker run --name ejudge -it hydai/ejudge-dev

$ # In /workdir, run demo directly
$ ./build/src/ejudge ./testcase ./lib/libaleth-interpreter.so
```

## How to build

```bash
git submodule update --init --recursive
mkdir build
cd build
cmake ..
make
```

After build, `ejudge` executable file will place at `build/src/ejudge`

## How to add testcase

### source file

The filename extension of the source file must be `.bin` or `.sol`, representing binary source and solidity source.

You can see binary source example at `testcase/example_binary` and solidity source example at `testcase/example_solidity`.

### json file

ejudge reads the input and expected output of the test data by reading the information of the json file. Each json file corresponds to a testcase.

Following are the objects we requested:

* `name`(string): Name of the testcase.
* `source`(string): Path of the source file. Note that the same source file can be used by different testcases.
* `input`(string): Input of the testcase.
* `expect`(string): Expected output of the testcase.

There is an extra object:
* `expect_code`(int): Expected code of the testcase. Note that if the object isn't added to json, it will be preset to `0`. The legal expected codes can be find in the `enum evmc_status_code` at `Third/EVMC/include/evmc/emvc.h`.

You can see example json file with `expect_code` at `testcase/example_binary/5000000_times_add.json`.

You can put the source file and json file anywhere in the testcases directory.

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

## Output

ejudge will output the information of VM, OS and each testcases. For each testcase, it will show testcase name, average runtime and speed.

Following are the execepted output when you run our testcase:
```
===================================TESTS=================================
|ID   | Testcase            | Average Runtime      | Speed              |
|-----| --------------------| ---------------------| -------------------|
|1    | ERC20 --evm-version | 0.29378801    ms/per | 207.1153278   MG/s |
|2    | 1000 times add      | 2.24086801    ms/per | 19.80571805   MG/s |
|3    | compare op          | 0.01974301    ms/per | 36.11404745   MG/s |
|4    | wrong output        | Fail! Output Miss Match!                  |
>>>VM:
0000000000000000000000000000000000000000000000000000000000abcdef
0000000000000000000000000000000000000000000000000000000000007122
<<<ECPECT
|5    | wrong expect_code   | Fail! Result Status Code Miss Match!     |
>>>VM:
EVMC_REVERT
EVMC_FAILURE
<<<ECPECT
|6    | Hello World         | 0.00361801    ms/per | 4.698715592   MG/s |
|7    | input               | 0.00304901    ms/per | 7.21545682    MG/s |
|8    | math op & SHA3 hash | 0.01291201    ms/per | 27.41633564   MG/s |
|9    | simple contract fun | 0.00911001    ms/per | 21.95387272   MG/s |
|10   | simple contract fun | 0.01043001    ms/per | 21.57236666   MG/s |
|11   | stack & memory size | 2.44055901    ms/per | 25.53021654   MG/s |
|12   | (Sol Builder)simple | 0.01014501    ms/per | 21.48839676   MG/s |
|13   | (Sol Builder)simple | 0.01013501    ms/per | 28.7123545    MG/s |
=========================================================================
```

We will run each testcase 100 times to calculate average runtime. If testcase's result code or output is not expected, the error message will be show like below.

There are two different error message:

1. Result Status Code Miss Match<br>If VM result code is not expected, we will show the difference between VM result code and expected result code.
2. Output Miss Match!<br>If VM output is not expected, we will show the difference between VM output code and expected output.

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
