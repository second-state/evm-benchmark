#include <Core/Benchmark.h>

#include <Common/Algorithm.h>
#include <Builder/Builder.h>
#include <Log/Color.h>
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

// Format
// Width = char[75] + '\n'
//  12345 123456789012345678901 1234567890123456789012 12345678901234567890
// |5    |21                   |22                    |20                  |
//

// TODO: remove this
static std::map<TestcaseState, std::string> TestcaseStateString = {
    {TestcaseState::Ready,              "Ready"},
    {TestcaseState::CompileFail,        "Compile Fail"},
    {TestcaseState::NoMatchedBuilder,   "No Matched Builder"},
    {TestcaseState::UnknownError,       "Unknown Error"}
};


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

        if( test.state != TestcaseState::Ready )
        {
            Log::Critical(dout()) << std::setw(22+20) << TestcaseStateString[test.state];
            dout()<<"|\n";
            continue;
        }
        
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
        evmc_context *context;
        for(int i=0 ; i < testtimes ; ++i)
        {
            std::chrono::nanoseconds temp;
            result = m_vm.execute(test.binary, msg, temp, context);
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

            auto log = reinterpret_cast<VirtualEVMCContent*>(context)->log;
            if(!(log==test.log))
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
            Log::Error(dout()) << std::setw(42) << "Fail! Result Status Code Miss Match!";
            dout()<< "|\n";
            dout() << ">>>>VM Result:\n";
            dout() << evmc_status_code_map.at(result.status_code) << "\n";
            dout() << evmc_status_code_map.at(test.expect_code) << "\n<<<ECPECT";
            dout() << std::setfill(' ');
        }
        else if( memcmp(result.output_data, test.expect.data(), test.expect.size()) != 0 )
        {
            Log::Error(dout()) << std::setw(42)<< "Fail! Output Miss Match!" ;
            dout()<< "|\n";

            dout() << ">>>VM Result:\n";
            for(size_t i=0;i<result.output_size;++i)
            {
                dout() << std::hex << std::setfill('0') << std::right;
                if( i < test.expect.size() && result.output_data[i] != test.expect[i] )
                    Log::Warning(dout()) << std::setw(2)  << (unsigned)result.output_data[i]; 
                else
                    dout() << std::setw(2)  << (unsigned)result.output_data[i]; 
            }
            dout()<<"\n";
            for(unsigned d:test.expect)
                dout() << std::hex << std::setw(2) << std::setfill('0') << std::right << d; 
            dout() << "\n<<<ECPECT";

            dout() << std::setfill(' ');
            all_accept = false;
        }
        else
        {
            Log::Error(dout()) << std::setw(42)<< "Fail! Log Miss Match! " << "|\n";

            dout() << ">>>>VM Result:\n";
            auto log = reinterpret_cast<VirtualEVMCContent*>(context)->log;
            for(auto l:log){
                dout() << "{\n";
                dout() << "   addr: " << uint8Arr2hexString(l.addr.bytes,20) << '\n';
                dout() << "   topics:\n";
                for(auto t:l.topics)
                {
                    dout() << "       " << uint8Arr2hexString(t.bytes,32) << '\n';
                }
                dout() << "   data: " << uint8Arr2hexString(l.data.data(),l.data.size()) << '\n';
                dout() << "}\n";
            }
            dout() << "------------------------------------------------------------------------------\n";
            for(auto l:test.log){
                dout() << "{\n";
                dout() << "   addr: "<<uint8Arr2hexString(l.addr.bytes,20) << '\n';
                dout() << "   topics:\n";
                for(auto t:l.topics)
                {
                    dout() << "       " << uint8Arr2hexString(t.bytes,32) << '\n';
                }
                dout() << "   data: " << uint8Arr2hexString(l.data.data(),l.data.size()) << '\n';
                dout() << "}\n";
            }
            dout() << "<<<ECPECT";

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
