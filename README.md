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

See more information from [tutorial](Tutorial.md)

## Run testcases

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
| ID  | Testcase            | Average Runtime     | Speed               |
| ----| --------------------| --------------------| --------------------|
| 1   | (Sol Builder)ERC20  | 0.30517101   ms/pre | 19819.05162    MG/s |
| 2   | ERC20 --evm-version | 0.23689801   ms/pre | 25685.31496    MG/s |
| 3   | 1000 times add      | 2.15405801   ms/pre | 2060.390193    MG/s |
| 4   | compare op          | 0.01756501   ms/pre | 4059.206343    MG/s |
| 5   | wrong output        | Fail! Output Miss Match!                  |
>>>VM Result:
0000000000000000000000000000000000000000000000000000000000abcdef
0000000000000000000000000000000000000000000000000000000000007122
<<<ECPECT
| 6   | wrong expect_code   | Fail! Result Status Code Miss Match!      |
>>>>VM Result:
EVMC_REVERT 2
EVMC_FAILURE 1
<<<ECPECT
| 7   | Hello World         | 0.00308501   ms/pre | 551.0516984    MG/s |
| 8   | input               | 0.00295301   ms/pre | 745.0025567    MG/s |
| 9   | math op & SHA3 hash | 0.01212301   ms/pre | 2920.066881    MG/s |
| 10  | simple contract fun | 0.00793801   ms/pre | 2519.523155    MG/s |
| 11  | simple contract fun | 0.00926801   ms/pre | 2427.705624    MG/s |
| 12  | stack & memory size | 2.34780201   ms/pre | 2653.886475    MG/s |
| 13  | (Sol Builder)event  | 0.00916601   ms/pre | 13026.38771    MG/s |
| 14  | (Sol Builder)log0   | 0.01026001   ms/pre | 8245.605998    MG/s |
| 15  | log0_emptyMem       | 0.00221201   ms/pre | 17224.1536     MG/s |
| 16  | (Sol Builder)log3   | 0.01413801   ms/pre | 14018.94609    MG/s |
| 17  | (Sol Builder)log3 e | Fail! Log Miss Match!                     |
>>>VM Result:
3d1ca41aedfa1a49a75bea323d2fc87227c87c166c4ef9bb9c7a14c1f3138dc4
1dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d49347
<<<ECPECT
| 18  | (Sol Builder)simple | 0.00951001   ms/pre | 2292.321459    MG/s |
.
.
.
| 624 | suicide0            | Fail! Result Status Code Miss Match!      |
>>>>VM Result:
EVMC_OUT_OF_GAS 3
EVMC_SUCCESS 0
<<<ECPECT
| 625 | suicideNotExistingA | Fail! Result Status Code Miss Match!      |
>>>>VM Result:
EVMC_OUT_OF_GAS 3
EVMC_SUCCESS 0
<<<ECPECT
| 626 | suicideSendEtherToM | Fail! Result Status Code Miss Match!      |
>>>>VM Result:
EVMC_OUT_OF_GAS 3
EVMC_SUCCESS 0
<<<ECPECT
| 627 | TestNameRegistrator | 0.00836501   ms/pre | 62582.11287    MG/s |
| 628 | suicide             | 0.00364001   ms/pre | 137417.2049    MG/s |
=========================================================================
```

We will run each testcase 100 times to calculate average runtime. If testcase's result code or output is not expected, the error message will be show like below.

There are two different error message:

1. Result Status Code Miss Match<br>If VM result code is not expected, we will show the difference between VM result code and expected result code.
2. Output Miss Match!<br>If VM output is not expected, we will show the difference between VM output code and expected output.

## Unimplemented functions

Becuase some functions in content table are not implemented, some tests are unavailible and the result maybe unpredictable.


* ```call``` : althrough opcode ```STATICCALL(0xfa)``` is working now, but it dose not check errors if existing any side effect. others opcode like ```call```, ```delegatecall```, ```create```, ```create2``` etc. are not working.

* ```selfdestruct``` : opcode ```SUICIDE``` is not working.

* ```copy_code``` : opcode ```EXTCODECOPY``` is not working.

* ```get_tx_context``` : all information like ```TIMESTAMP```, ```NUMBER```, ```DIFFICULTY```  pass by ```TxContext``` are not working.

* ```get_block_hash``` : opcode ```BLOCKHASH``` is not working.
