#include <Core/Benchmark.h>

#include <Builder/Builder.h>
#include <Testcase/TestcaseLoder.h>
#include <evmc/evmc.h>

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
        dout() << "uname error:" << errno << std::endl;
    }
#endif
}

bool Benchmark::genEnvInfo()
{
    int counter;

    // Project Information
    dout() << "=== Benchmark\n";
    dout() << "ejudge V1.0.0\n";
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
    dout() << "Testcases: " << m_casesloder.testcases().size() << std::endl;

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
    int testtimes = 100;
    bool all_accept = true;

    dout() << std::setprecision(10);
    dout() << "===================================TESTS=================================\n";
    dout() << "|";
    dout() << std::setw(5)  << std::left << "ID" << "| " ;
    dout() << std::setw(20) << std::left << "Testcase" << "| ";
    dout() << std::setw(10+11) << std::left << "Average Runtime" << "| ";
    dout() << std::setw(10+9) << std::left << "Speed" << "| ";
    dout() << std::endl;

    dout() << std::setfill('-');
    dout() << "|";
    dout() << std::setw(5)  << "" << "| " ;
    dout() << std::setw(20) << "" << "| ";
    dout() << std::setw(10+11) << "" << "| ";
    dout() << std::setw(10+9) << "" << "| ";
    dout() << std::endl;

    dout() << std::setfill(' ');
    for(auto const &test : m_casesloder.testcases())
    {
        //Print title
        dout() << "|";
        dout() << std::left << std::setw(5) << std::dec << ++counter << "| " ;
        dout() << std::setw(20) << std::left << test.name.substr(0,19) << "| ";
        
        bool accept = true;
        std::chrono::nanoseconds runtime {0};

        evmc_message msg = {};
        const evmc_address addr = {{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x4A,0x69,0x6E,0x6B,0x65,0x6C,0x61}};
        const evmc_uint256be value = {{0, 0}};

        msg.sender = addr;
        msg.destination = addr;
        msg.value = value;
        msg.input_data = test.input.data();
        msg.input_size = test.input.size();
        msg.gas = 100000000;
        msg.depth = 0;

        evmc_result result;
        for(int i=0 ; i < testtimes ; ++i)
        {
            std::chrono::nanoseconds temp;
            result = m_vm.execute(test.binary, msg, temp);
            runtime += temp;

            if( result.status_code != test.expect_code )
            {
                accept = false;
                break;
            }

            if( test.expect_code != EVMC_SUCCESS )
            {
                break;
            }

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
            double runtime_once_ms = 1.0 * ( runtime.count() + 1 ) / testtimes / 1E9 * 1000;
            double gas_speed = 1.0 * gas_used / 1E6 / ( 1.0 * (runtime.count()+1) / testtimes / 1E9 );
            dout() << std::setw(13) << runtime_once_ms  <<" ms/per | ";
            dout() << std::setw(13) << gas_speed  <<" MG/s | ";
        }
        else if( result.status_code != test.expect_code )
        {
            dout() << std::setw(41)<< "Fail! Result Status Code Miss Match!" << "|\n";
            dout() << ">>>VM:\n";
            dout() << evmc_status_code_map.at(result.status_code) << "\n";
            dout() << evmc_status_code_map.at(test.expect_code) << "\n<<<ECPECT";
            dout() << std::setfill(' ');
        }
        else
        {
            dout() << std::setw(42)<< "Fail! Output Miss Match! " << "|\n";

            dout() << ">>>VM:\n";
            for(int i=0;i<result.output_size;++i)
                dout() << std::hex << std::setw(2) << std::setfill('0') << std::right << (unsigned)result.output_data[i]; 
            dout()<<"\n";
            for(unsigned d:test.expect)
                dout() << std::hex << std::setw(2) << std::setfill('0') << std::right << d; 
            dout() << "\n<<<ECPECT";

            dout() << std::setfill(' ');
            all_accept = false;
        }
        
        if( result.release )
            result.release(&result);
        dout() << "\n";
    }
    dout() << "=========================================================================\n";
    dout().unsetf(std::ios::fixed);
    return all_accept;
}

std::ostream& Benchmark::dout()
{
    return m_stdout;
}

std::ostream& Benchmark::derr()
{
    return m_stderr;
}
