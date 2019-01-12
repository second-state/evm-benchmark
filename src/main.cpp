#include <Testcase/TestcaseLoder.h>
#include <Core/Warper.h>
#include <Common/Algorithm.h>

#include <iostream>
#include <vector>

int main()
{
    TestcaseLoader tl("test");
    tl.load();

    VMInterface vim("./bin/libaleth-interpreter.so");
    vim.showVMInformation(std::cout);

    std::string test = "7FFFFFFF7122FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFABCDE600052596000F3";
    std::vector<uint8_t> code = hex2Uint8Vec(test), in;

    vim.execute(code, in);
}