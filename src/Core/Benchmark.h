#pragma once

#include <Builder/Builder.h>
#include <Core/Warper.h>
#include <Testcase/TestcaseLoder.h>
#include <evmc/evmc.h>

#include <ostream>
#include <string>

class Benchmark
{
public:
    Benchmark(std::string _testsbase, std::string _vmpath, std::ostream &_out, std::ostream &_errout);

    void addBuilder(Builder *_builder);
    bool run();

private:
    bool prepare();
    bool genEnvInfo();
    bool runTests();

    void showOSInfo();

    std::ostream& dout();
    std::ostream& derr();

    TestcaseLoader m_casesloder;
    VMInterface m_vm;

    std::vector<std::string> m_builder_str;

    std::string m_testcasebase;
    std::string m_vmpath;
    std::ostream &m_stdout;
    std::ostream &m_stderr;
};
