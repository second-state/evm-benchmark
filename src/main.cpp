#include <Builder/Builder.h>
#include <Testcase/TestcaseLoder.h>
#include <Core/Warper.h>
#include <Core/Benchmark.h>
#include <Common/Algorithm.h>

#include <iostream>
#include <vector>

int main()
{
    std::string testcase_base = "./test";
    std::string evmc_path = "./bin/libaleth-interpreter.so";

    Benchmark bm(testcase_base, evmc_path, std::cout, std::cerr);

    bm.addBuilder(new BinaryBuilder);
    //bm.addBuilder(new LityBuilder);
    //bm.addBuilder(new SoldityBuilder);

    bm.run();

    std::cout<<"====";

    VMInterface vim("./bin/libaleth-interpreter.so");
    vim.showVMInformation(std::cout);

    std::string test = "7FFFFFFF7122FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFABCDE600052596000F3";
    std::vector<uint8_t> code = hex2Uint8Vec(test), in;

    vim.execute(code, in);
}