#include <Builder/Builder.h>
#include <Testcase/TestcaseLoder.h>
#include <Core/Warper.h>
#include <Core/Benchmark.h>
#include <Common/Algorithm.h>
#include <CLI/CLI.hpp>

#include <iostream>
#include <vector>

int main(int argc, char *argv[])
{
    CLI::App app;

    std::string testcase_base;
    app.add_option("TESTCASES_PATH", testcase_base, "Path of testcase folder")->required();
    
    std::string evmc_path;
    app.add_option("VMFILE", evmc_path, "Path of evmc VM file")->required();
    
    CLI11_PARSE(app, argc, argv);

    Benchmark bm(testcase_base, evmc_path, std::cout, std::cerr);

    bm.addBuilder(new BinaryBuilder);
    //bm.addBuilder(new LityBuilder);
    bm.addBuilder(new SoldityBuilder);

    bm.run();

}