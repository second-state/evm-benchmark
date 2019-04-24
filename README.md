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
* `expect_log`(string): An expect hex encoded hash of the rlp encoded log entries (see https://github.com/ethereum/py-evm/blob/7a96fa3a2b00af9bea189444d88a3cce6a6be05f/eth/tools/_utils/hashing.py#L8-L16)

There is an extra object:
* `contract_name`(string): Name of the contract of solidity file. Note that If you want to build source file by solidity builder, you must add this object.
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
|1    | (Sol Builder)ERC20  | 0.30263501    ms/per | 199.8512994   MG/s |
|2    | ERC20 --evm-version | 0.25764001    ms/per | 236.1744979   MG/s |
|3    | 1000 times add      | 2.01893001    ms/per | 21.98293144   MG/s |
|4    | compare op          | 0.01885601    ms/per | 37.8128777    MG/s |
|5    | wrong output        | Fail! Output Miss Match!                  |
>>>VM Result:
0000000000000000000000000000000000000000000000000000000000abcdef
0000000000000000000000000000000000000000000000000000000000007122
<<<ECPECT
|6    | wrong expect_code   | Fail! Result Status Code Miss Match!      |
>>>>VM Result:
EVMC_REVERT
EVMC_FAILURE
<<<ECPECT
|7    | Hello World         | 0.00338101    ms/per | 5.028083324   MG/s |
|8    | input               | 0.00287701    ms/per | 7.646827783   MG/s |
|9    | math op & SHA3 hash | 0.01287301    ms/per | 27.49939602   MG/s |
|10   | simple contract fun | 0.00983301    ms/per | 20.33965185   MG/s |
|11   | simple contract fun | 0.01107001    ms/per | 20.32518489   MG/s |
|12   | stack & memory size | 3.28402101    ms/per | 18.97308203   MG/s |
|13   | (Sol Builder)event  | 0.00983701    ms/per | 121.3783457   MG/s |
|14   | (Sol Builder)log0   | 0.01002401    ms/per | 84.39736193   MG/s |
|15   | (Sol Builder)log3   | 0.02053501    ms/per | 96.51809276   MG/s |
|16   | (Sol Builder)log3 e | Fail! Log Miss Match!                     |
>>>VM Result:
3d1ca41aedfa1a49a75bea323d2fc87227c87c166c4ef9bb9c7a14c1f3138dc4
1dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d49347
<<<ECPECT
|17   | (Sol Builder)simple | 0.01095801    ms/per | 19.89412311   MG/s |
|18   | (Sol Builder)simple | 0.01053301    ms/per | 27.62743034   MG/s |
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
