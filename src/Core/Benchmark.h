#pragma once

#include <Testcase/TestcaseLoder.h>
#include <Builder/Builder.h>

#include <ostream>
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
    void showOSInfo();

    std::ostream& dout();
    std::ostream& derr();

    TestcaseLoader m_casesloder;
    VMInterface m_vm;

    std::set<std::string> m_allowed_exts;

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

    for(std::string str:_builder->acceptExtensions())
        m_allowed_exts.insert(str);
}

bool Benchmark::run()
{
    return prepare() && genEnvInfo() && true;
}

bool Benchmark::prepare()
{
    m_casesloder.load(m_testcasebase);
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
        dout()<<"RELEASE: ?\n";
        dout()<<"VERSION: ?\n";
#else
    utsname ust;
    if( uname(&ust) == 0 )
    {
        dout()<<"OS     : "<<ust.sysname<<"\n";
        dout()<<"RELEASE: "<<ust.release<<'\n';
        dout()<<"VERSION: "<<ust.version<<"\n";
    }
    else
    {
        dout() << "uname error:" << errno <<endl;
    }
#endif
}

bool Benchmark::genEnvInfo()
{
    // Project Information
    dout() << "=== Benchmark\n";
    dout() << "AAAA V0.01\n";
    dout() << "Build:" << __DATE__ " " __TIME__ "\n";
    m_vm.showVMInformation(dout());

    // Envirment Info
    showOSInfo();

    // Testcases
    dout() << "=== Testcases\n";
    dout() << "EXT:";
    for(const auto &str : m_allowed_exts)
        dout()<<' '<<str;
    dout() <<'\n';
    dout() << "ALL: " << m_casesloder.testcases().size()<<endl;
    for(const auto &tmp:m_casesloder.testcases())
    {
        dout() << "   " << tmp.m_path << '\n';
    }

    // Builder Info
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
