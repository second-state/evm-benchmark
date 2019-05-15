#include <Core/Benchmark.h>

#include <Common/Algorithm.h>
#include <Common/keccak-tiny.h>
#include <Builder/Builder.h>
#include <Log/Color.h>
#include <Testcase/TestcaseLoder.h>
#include <evmc/evmc.h>
#include <RLP/RLP.h>

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

Benchmark::Benchmark(std::string _testsbase, std::string _vmpath, std::ostream &_out, std::ostream &_err) : m_testcasebase(_testsbase), m_vmpath(_vmpath), m_vm(_vmpath), m_stdout(_out), m_stderr(_err)
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
    if (!m_vm.isOpen())
    {
        derr() << "Can not open VM File!";
        return false;
    }
    return true;
}

void Benchmark::showOSInfo()
{
    dout() << "=== OS Information\n";
#ifdef _WIN32
    dout() << "OS     : Windows\n";
    dout() << "Release: ?\n";
    dout() << "Version: ?\n";
#else
    utsname ust;
    if (uname(&ust) == 0)
    {
        dout() << "OS     : " << ust.sysname << "\n";
        dout() << "Release: " << ust.release << '\n';
        dout() << "Version: " << ust.version << "\n";
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
    for (const auto &str : m_builder_str)
        dout() << "  " << counter++ << "). " << str << '\n';
    dout() << '\n';
    dout() << "Testcases: " << m_casesloder.testcases().size() << std::endl;

    counter = 1;
    for (const auto &tmp : m_casesloder.testcases())
    {
        dout() << "  " << counter++ << "). " << tmp.json_path << '\n';
        dout() << "  "
               << " "
               << "     " << tmp.source_path << '\n';
    }

    // Builder Info
    return true;
}

// Format
// Width = char[75] + '\n'
//  12345 123456789012345678901 1234567890123456789012 12345678901234567890
// |5    |21                   |22                    |20                  |
//

// TODO: move TestcaseStateString to better position
static std::map<TestcaseState, std::string> TestcaseStateString = {
    {TestcaseState::Ready, "Ready"},
    {TestcaseState::CompileFail, "Compile Fail"},
    {TestcaseState::NoMatchedBuilder, "No Matched Builder"},
    {TestcaseState::UnknownError, "Unknown Error"}
};


enum class STRID
{
    MSPER,
    MGS,
    RESULT_MISSMATCH,
    OUTPUT_MISSMATCH,
    LOG_MISSMATCH
};

static std::map<STRID, std::string> LANG = {
    {STRID::MSPER,  " ms/pre "},
    {STRID::MGS,    " MG/s "},
    {STRID::RESULT_MISSMATCH, "Fail! Result Status Code Miss Match!"},
    {STRID::OUTPUT_MISSMATCH, "Fail! Output Miss Match!"},
    {STRID::LOG_MISSMATCH, "Fail! Log Miss Match!"}
};

bool Benchmark::runTests()
{
    const int EDGE_W = 2;
    const int COL1_W = 4;
    const int COL2_W = 20;
    const int COL3_W = 20;
    const int COL4_W = 20;
    int counter = 0;
    bool all_accept = true;

    dout() << std::setprecision(10);
    dout() << "===================================TESTS=================================\n";
    dout() << "| ";
    dout() << std::setw(COL1_W) << std::left << "ID"
           << "| ";
    dout() << std::setw(COL2_W) << std::left << "Testcase"
           << "| ";
    dout() << std::setw(COL3_W) << std::left << "Average Runtime"
           << "| ";
    dout() << std::setw(COL4_W) << std::left << "Speed"
           << "| ";
    dout() << std::endl;

    dout() << std::setfill('-');
    dout() << "| ";
    dout() << std::setw(COL1_W) << ""
           << "| ";
    dout() << std::setw(COL2_W) << ""
           << "| ";
    dout() << std::setw(COL3_W) << ""
           << "| ";
    dout() << std::setw(COL4_W) << ""
           << "| ";
    dout() << std::endl;

    dout() << std::setfill(' ');
    for (auto const &test : m_casesloder.testcases())
    {
        //Print title
        dout() << "| ";
        dout() << std::left << std::setw(COL1_W) << std::dec << ++counter << "| ";
        dout() << std::left << std::setw(COL2_W) << test.name.substr(0, COL2_W - 1) << "| ";

        if (test.state != TestcaseState::Ready)
        {
            Log::Critical(dout()) << std::setw(COL3_W + EDGE_W + COL4_W) << TestcaseStateString[test.state];
            dout() << "|\n";
            continue;
        }

        bool accept = true;
        std::chrono::nanoseconds runtime{0};

        evmc_message msg;

        int64_t gas_used = 0;

        evmc_result result;
        evmc_context *context;
        vector<uint8_t> logs_hash(32);
        for (int i = 0; i < test.testtimes; ++i)
        {
            std::chrono::nanoseconds temp;
            result = m_vm.execute(test, msg, temp, context);
            runtime += temp;

            if (result.status_code != test.expect_code)
            {
                accept = false;
                break;
            }

            if (test.expect_code != EVMC_SUCCESS)
            {
                break;
            }

            if (result.output_size != test.out.size())
            {
                accept = false;
                break;
            }

            if (memcmp(result.output_data, test.out.data(), test.out.size()) != 0)
            {
                accept = false;
                break;
            }

            auto logEncode = reinterpret_cast<VirtualEVMCContent *>(context)->getLogRLPencode();
            keccak_256(logs_hash.data(), logs_hash.size(), logEncode.data(), logEncode.size());
            if (logs_hash != test.logs)
            {
                accept = false;
                break;
            }

            gas_used += msg.gas - result.gas_left;
        }
        if (accept)
        {
            double runtime_once_ms = 1.0 * (runtime.count() + 1) / test.testtimes / 1E9 * 1000;
            double gas_speed = 1.0 * gas_used / 1E6 / (1.0 * (runtime.count() + 1) / test.testtimes / 1E9);
            dout() << std::setw(COL3_W - LANG[STRID::MSPER].size()) << runtime_once_ms << LANG[STRID::MSPER] << "| ";
            dout() << std::setw(COL4_W - LANG[STRID::MGS].size()) << gas_speed << LANG[STRID::MGS] << "| ";
        }
        else if (result.status_code != test.expect_code)
        {
            Log::Error(dout()) << std::setw(COL3_W + EDGE_W + COL4_W) << LANG[STRID::RESULT_MISSMATCH];
            dout() << "|\n";
            dout() << ">>>>VM Result:\n";
            Log::Warning(dout()) << evmc_status_code_map.at(result.status_code) << ' '<< result.status_code << "\n";
            dout() << evmc_status_code_map.at(test.expect_code) << ' '<< test.expect_code << "\n<<<ECPECT";
            dout() << std::setfill(' ');
        }
        else if (memcmp(result.output_data, test.out.data(), test.out.size()) != 0)
        {
            Log::Error(dout()) << std::setw(COL3_W + EDGE_W + COL4_W) <<LANG[STRID::OUTPUT_MISSMATCH];
            dout() << "|\n";

            dout() << ">>>VM Result:\n";
            for (size_t i = 0; i < result.output_size; ++i)
            {
                dout() << std::hex << std::setfill('0') << std::right;
                if (i < test.out.size() && result.output_data[i] != test.out[i])
                    Log::Warning(dout()) << std::setw(2) << (unsigned)result.output_data[i];
                else
                    dout() << std::setw(2) << (unsigned)result.output_data[i];
            }
            dout() << "\n";
            for (unsigned d : test.out)
                dout() << std::hex << std::setw(2) << std::setfill('0') << std::right << d;
            dout() << "\n<<<ECPECT";

            dout() << std::setfill(' ');
            all_accept = false;
        }
        else
        {
            Log::Error(dout()) << std::setw(COL3_W + EDGE_W + COL4_W) << LANG[STRID::LOG_MISSMATCH]
                               << "|\n";

            dout() << ">>>VM Result:\n";
            for (size_t i = 0; i < logs_hash.size(); ++i)
            {
                dout() << std::hex << std::setfill('0') << std::right;
                if (i < test.logs.size() && logs_hash[i] != test.logs[i])
                    Log::Warning(dout()) << std::setw(2) << (unsigned)logs_hash[i];
                else
                    dout() << std::setw(2) << (unsigned)logs_hash[i];
            }
            dout() << "\n";
            for (unsigned d : test.logs)
                dout() << std::hex << std::setw(2) << std::setfill('0') << std::right << d;
            dout() << "\n<<<ECPECT";

            dout() << std::setfill(' ');
            all_accept = false;
        }

        if (result.release)
            result.release(&result);
        dout() << "\n";
    }
    dout() << "=========================================================================\n";
    dout().unsetf(std::ios::fixed);
    return all_accept;
}

std::ostream &Benchmark::dout()
{
    return m_stdout;
}

std::ostream &Benchmark::derr()
{
    return m_stderr;
}
