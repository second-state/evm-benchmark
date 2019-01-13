#include <Testcase/TestcaseLoder.h>

#include <algorithm>
#include <string>
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <iostream>

#include <Common/Algorithm.h>
#include <Common/FileUtils.h>
#include <Filesystem/FileIterator.h>
#include <Testcase/Testcase.h>
#include <Builder/Builder.h>

namespace fs = std::filesystem;

TestcaseLoader::TestcaseLoader()
{

}

TestcaseLoader::~TestcaseLoader()
{
    for(Builder *ptr:m_builder)
        delete ptr;
}

void TestcaseLoader::addBuilder(Builder *_builder)
{
    m_builder.emplace_back(_builder);
}

void TestcaseLoader::clear()
{
    m_testcases.clear();
}

void TestcaseLoader::load(std::string _base)
{
    FileIterator fit;
    fit.visit(_base, [&](fs::path file){
        if( file.has_extension() )
        {
            const std::string ext = file.extension().string();
            const std::string full_name = file.string();
            const std::string ans_name = full_name.substr(0, full_name.size() - ext.size()) + ".ans";

            for(Builder *ptr:m_builder)
            {
                if( contain( ptr->acceptExtensions(), ext ) )
                {
                    std::string source = GetFileContent(full_name);
                    std::string answer = GetFileContent(ans_name);

                    if( source.size() == 0 )
                        continue;

                    if( ptr->build(source) )
                    {
                        m_testcases.emplace_back( ptr->getTestcase() );
                        m_testcases.back().m_path = full_name;
                    }
                    else
                    {
                        std::cout<<"Build "<<full_name<<" with "<<ptr->getClassName()<<" FAIL!"<<std::endl;
                    }
                }
            }
        }
    });
}

const std::vector<Testcase> &TestcaseLoader::testcases()
{
    return m_testcases;
}