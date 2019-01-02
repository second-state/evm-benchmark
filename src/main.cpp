#include<Testcase/TestcaseLoder.h>
#include<Core/Warper.h>

#include<iostream>

int main()
{
    TestcaseLoader tl("test");
    tl.load();

    VMInterface vim("./bin/libaleth-interpreter.so");
    vim.showVMInformation(std::cout);
}