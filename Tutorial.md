ejudge tutorial
==========

This tutorial help developers to add a new testcase for ejudge

## Add a binary code testcase

To add a binary code testcase, a binary code file and a config file in json should be prepared. Use Hello World for example:

HelloWorld.bin :

```bin
62ABCDEF600052596000F3
```

HelloWorld.json :

```json
{
    "Hello World" : {
        "exec" : {
            "address" : "000000000000000000000000004A696E6B656C61",
            "caller" : "000000000000000000000000004A696E6B656C61",
            "codeFile" : "HelloWorld.bin",
            "data" : "",
            "value" : "0"
        },
        "logs" : "1dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d49347",
        "out" : "0000000000000000000000000000000000000000000000000000000000abcdef"
    }
}
```

Arguments meaning :

* ```Hello World``` : Testcase name, ejudge will show first 19 character as testcase title.
  * ```exec``` : contain some runtime information.
    * ```address``` : set address for binary code.
    * ```caller``` : set caller address.
    * ```codeFile```: set input binary code file name, here use ```HelloWorld.bin``` for example.
    * ```data``` : set runtime input
  * ```logs``` : set expect log output.
  *  ```logs``` : set expect program output.

Then, put both files in a folder named ```testcase```.

```bin
./testcase/HelloWorld.bin
./testcase/HelloWorld.json
````

Execute ejudge

```bash
ejudge ./path/to/testcase/ ./path/to/vm/suchas/libaleth-interpreter.so
```

If it is successful, ejudge will show information like this :

```text
=== Benchmark
ejudge V1.0.0
Build:May  4 2019 18:44:18
=== VM [../bin/libaleth-interpreter.so]
ABI    : 6
Name   : interpreter
Version: 1.5.0-alpha.7
=== OS Information
OS     : Linux
Release: 4.4.0-17763-Microsoft
Version: #379-Microsoft Wed Mar 06 19:16:00 PST 2019
=== Testcases
Builder  :
  1). BinaryBuilder
  2). SoldityBuilder

Testcases: 1
  1). ./testcase/HelloWorld.json
        /mnt/d/User/Documents/Git/EthBrenchmark/example/testcase/HelloWorld.bin
===================================TESTS=================================
| ID  | Testcase            | Average Runtime     | Speed               |
| ----| --------------------| --------------------| --------------------|
| 1   | Hello World         | 0.00355201   ms/pre | 4.786022562    MG/s |
=========================================================================
```

## Add a solidity testcase

To add a solidity testcase, a solidity file and a config file in json should be prepared. Use Hello World for example:

```sol
pragma solidity ^0.5.0;

contract HelloWorld {
  function helloWorld() external pure returns (uint a) {
	a = 0x1234;
  }
}

//HelloWorld c605f76c
```

HelloWorld.json :

```json
{
    "(Sol Builder)simple contract function A" : {
        "contract_name" : "HelloWorld",
        "exec" : {
            "address" : "000000000000000000000000004A696E6B656C61",
            "caller" : "000000000000000000000000004A696E6B656C61",
            "codeFile" : "HelloWorld.sol",
            "data" : "c605f76c",
            "value" : "0"
        },
        "logs" : "1dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d49347",
        "out" : "0000000000000000000000000000000000000000000000000000000000001234"
    }
}
```

Most Arguments just like binary code testcase format, but there are something shoudld be notice :

* ```codeFile``` : If use  ```*.sol``` for input, ejudge will build it with solidity compiler to generate binary code.
* ```data``` : usually a contract required a input to determine  which function should be execute. Some tools like Remix can provide such information in simple method.

Then, put both files in a folder named ```testcase```.

```bin
./testcase/HelloWorld.sol
./testcase/HelloWorld.json
````

Execute ejudge

```bash
export SOLIDITYC=./path/to/solidity_compiler/like/solc
ejudge ./path/to/testcase/ ./path/to/vm/suchas/libaleth-interpreter.so
```

If it is successful, ejudge will show information like this :

```text
solc, the solidity compiler commandline interface
Version: 0.5.2+commit.1df8f40c.Linux.g++
Compiler run successful. Artifact(s) can be found in directory ./.
=== Benchmark
ejudge V1.0.0
Build:May  4 2019 18:44:18
=== VM [../bin/libaleth-interpreter.so]
ABI    : 6
Name   : interpreter
Version: 1.5.0-alpha.7
=== OS Information
OS     : Linux
Release: 4.4.0-17763-Microsoft
Version: #379-Microsoft Wed Mar 06 19:16:00 PST 2019
=== Testcases
Builder  :
  1). BinaryBuilder
  2). SoldityBuilder

Testcases: 1
  1). ./testcase/HelloWorld.json
        /mnt/d/User/Documents/Git/EthBrenchmark/example/testcase/HelloWorld.sol
===================================TESTS=================================
| ID  | Testcase            | Average Runtime     | Speed               |
| ----| --------------------| --------------------| --------------------|
| 1   | (Sol Builder)simple | 0.14998501   ms/pre | 1.306797259    MG/s |
=========================================================================
```

* Quick fix : if result is Compile Fail, it meaning ```SOLIDITYC``` is not correctly. Please check compiler information.

```
/mnt/d/User/Documents/Git/EthBrenchmark/example/scripts/SoldityBuilder.sh: line 9: ../bin/solc: No such file or directory

or

/mnt/d/User/Documents/Git/EthBrenchmark/example/scripts/SoldityBuilder.sh: line 9: solc: command not found

...
| 1   | (Sol Builder)simple | Compile Fail                             |
..
```