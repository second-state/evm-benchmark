#pragma once

#include <Builder/Builder.h>
#include <Testcase/TestcaseLoder.h>
#include <Third/EVMC/include/evmc/evmc.h>

#include <chrono>
#include <cstring>
#include <ctime>
#include <ostream>
#include <iomanip>
#include <string>
#include <set>

#ifdef _WIN32
    #include <Windows.h>
#else
    #include <sys/utsname.h>
    #include <errno.h>
#endif

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

Benchmark::Benchmark(std::string _testsbase, std::string _vmpath, std::ostream &_out, std::ostream &_err):
    m_testcasebase(_testsbase), m_vmpath(_vmpath), m_vm(_vmpath), m_stdout(_out), m_stderr(_err)
{
    // Empty statment
}

void Benchmark::addBuilder(Builder *_builder)
{
    m_casesloder.addBuilder(_builder);
    m_builder_str.emplace_back(_builder->getClassName());
}

bool Benchmark::run()
{
    return prepare() && genEnvInfo() && runTests();
}

bool Benchmark::prepare()
{
    m_casesloder.load(m_testcasebase, derr());
    if( !m_vm.isOpen() )
    {
        derr() << "Can not open VM File!";
        return false;
    }
    return true;
}

void Benchmark::showOSInfo()
{
    dout()<<"=== OS Information\n";
#ifdef _WIN32
        dout()<<"OS     : Windows\n";
        dout()<<"Release: ?\n";
        dout()<<"Version: ?\n";
#else
    utsname ust;
    if( uname(&ust) == 0 )
    {
        dout()<<"OS     : "<<ust.sysname<<"\n";
        dout()<<"Release: "<<ust.release<<'\n';
        dout()<<"Version: "<<ust.version<<"\n";
    }
    else
    {
        dout() << "uname error:" << errno <<endl;
    }
#endif
}

bool Benchmark::genEnvInfo()
{
    int counter;

    // Project Information
    dout() << "=== Benchmark\n";
    dout() << "AAAA V0.01\n";
    dout() << "Build:" << __DATE__ " " __TIME__ "\n";
    m_vm.showVMInformation(dout());

    // Envirment Info
    showOSInfo();

    // Testcases
    dout() << "=== Testcases\n";
    dout() << "Builder  :\n";

    counter = 1;
    for(const auto &str : m_builder_str)
        dout() << "  " << counter++ << "). " << str << '\n';
    dout() <<'\n';
    dout() << "Testcases: " << m_casesloder.testcases().size()<<endl;

    counter = 1;
    for(const auto &tmp:m_casesloder.testcases())
    {
        dout() << "  " << counter++ << "). " << tmp.json_path << '\n';
        dout() << "  " << " " << "     " << tmp.source_path << '\n';
    }


    // Builder Info
    return true;
}

bool Benchmark::runTests()
{
    int counter = 0;
    int testtimes = 100000;
    dout() << "===============TESTS==================\n";
    for(auto const &test : m_casesloder.testcases())
    {
        dout() << ++counter << "\t| " <<  test.name << "\t| ";
        
        bool accept = true;
        time_t runtime = 0;

        evmc_message msg = {};
        const evmc_address addr = {{0, 1, 2}};
        const evmc_uint256be value = {{1, 0}};

        msg.sender = addr;
        msg.destination = addr;
        msg.value = value;
        msg.input_data = test.input.data();
        msg.input_size = test.input.size();
        msg.gas = 7120000;
        msg.depth = 1024;

        evmc_result result;
        for(int i=0 ; i < testtimes ; ++i)
        {
            time_t times;
            result = m_vm.execute(test.binary, msg, times);
            runtime += times;

            if( result.output_size != test.expect.size() )
            {
                accept = false;
                break;
            }

            if( memcmp(result.output_data, test.expect.data(), test.expect.size()) != 0 )
            {
                accept = false;
                break;
            }
        }
        
        if( accept )
        {
            auto gas_used = msg.gas - result.gas_left;
            dout() << 1.0 * ( runtime + 1) / testtimes / CLOCKS_PER_SEC * 1000  <<" ms/per | ";
            dout() << 1.0 * gas_used / 1E6 / ( 1.0 * (runtime+1) / testtimes / CLOCKS_PER_SEC )  <<" MG/s | ";
        }
        else
        {
            dout() << "\nFail! Output Miss Match! \t| ";

            dout() << ">>>>VM:\n";
            for(int i=0;i<result.output_size;++i)
                dout() << std::hex << std::setw(2) << std::setfill('0') << (unsigned)result.output_data[i]; 
            dout()<<"\n";
            for(unsigned d:test.expect)
                dout() << std::hex << std::setw(2) << std::setfill('0') << d; 
            dout() << "\n<<<ECPECT\n";
            return false;
        }

        dout() << "\n";
    }
    return true;
}

std::ostream& Benchmark::dout()
{
    return m_stdout;
}

std::ostream& Benchmark::derr()
{
    return m_stderr;
}
