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
Testcases: 12
  1). ../../testcase/ERC20/ERC20.json
        ../../testcase/ERC20/ERC20.bin
  2). ../../testcase/example_binary/5000000_times_add.json
        ../../testcase/example_binary/5000000_times_add.bin
  3). ../../testcase/example_binary/compareOperators.json
        ../../testcase/example_binary/compareOperators.bin
  4). ../../testcase/example_binary/HelloWorld.json
        ../../testcase/example_binary/HelloWorld.bin
  5). ../../testcase/example_binary/input.json
        ../../testcase/example_binary/input.bin
  6). ../../testcase/example_binary/math_binary_OP_with_SHA3_and_gas.json
        ../../testcase/example_binary/math_binary_OP_with_SHA3_and_gas.bin
  7). ../../testcase/example_binary/result_code_error.json
        ../../testcase/example_binary/5000000_times_add.bin
  8). ../../testcase/example_binary/simple_contract_A.json
        ../../testcase/example_binary/simple_contract.bin
  9). ../../testcase/example_binary/simple_contract_B.json
        ../../testcase/example_binary/simple_contract.bin
  10). ../../testcase/example_binary/stackMemorySize.json
        ../../testcase/example_binary/stackMemorySize.bin
  11). ../../testcase/example_solidity/simple_contract_A.json
        ../../testcase/example_solidity/simple_contract.sol
  12). ../../testcase/example_solidity/simple_contract_B.json
        ../../testcase/example_solidity/simple_contract.sol
===================================TESTS=================================
|ID   | Testcase            | Average Runtime      | Speed              |
|-----| --------------------| ---------------------| -------------------|
|1    | ERC20 --evm-version | 0.25319401    ms/per | 240.3216411   MG/s |
|2    | 5000000 times add   | 48.95083701   ms/per | 2042.865988   MG/s |
|3    | compare op          | 0.01840801    ms/per | 38.73313845   MG/s |
|4    | Hello World         | 0.00291901    ms/per | 5.823892347   MG/s |
|5    | input               | 0.00262101    ms/per | 8.393710821   MG/s |
|6    | math op & SHA3 hash | Fail! Output Miss Match!                  |
>>>VM:
0afa926dd80666aad5c8300ee7e35c50f3ee1d5e930ff36cbc936b6f57abb04d
0afa926dd80666aad5c8300ee7e35c50f3ee1d5e930ff36cbc9385a7920dd04d
<<<ECPECT
|7    | wrong expect_code   | Fail! Result Status Code Miss Match!     |
>>>VM:
EVMC_OUT_OF_GAS
EVMC_FAILURE
<<<ECPECT
|8    | simple contract fun | 0.00762201    ms/per | 26.23979764   MG/s |
|9    | simple contract fun | 0.00821901    ms/per | 27.37555983   MG/s |
|10   | stack & memory size | 2.19948901    ms/per | 28.32839797   MG/s |
|11   | (Sol Builder)simple | 0.00740701    ms/per | 29.431579     MG/s |
|12   | (Sol Builder)simple | 0.00829701    ms/per | 35.07287565   MG/s |
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
