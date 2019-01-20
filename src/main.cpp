#include <Builder/Builder.h>
#include <Testcase/TestcaseLoder.h>
#include <Core/Warper.h>
#include <Core/Benchmark.h>
#include <Common/Algorithm.h>

#include <iostream>
#include <vector>

void help()
{
    cout << "./ejudge TESTCASES_PATH VMFILE" << endl;
}

int main(int argc, char *argv[])
{
    if( argc != 3 )
    {
        help();
        return 0;
    }

    std::string testcase_base = argv[1];
    std::string evmc_path = argv[2];

    Benchmark bm(testcase_base, evmc_path, std::cout, std::cerr);

    bm.addBuilder(new BinaryBuilder);
    //bm.addBuilder(new LityBuilder);
    bm.addBuilder(new SoldityBuilder);

    bm.run();

}