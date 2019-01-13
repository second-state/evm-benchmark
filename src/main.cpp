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
    
}