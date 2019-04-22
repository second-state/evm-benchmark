#include <Testcase/TestcaseLoder.h>
#include <nlohmann/json.hpp>
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
#include <Log/Color.h>
#include <Testcase/Testcase.h>
#include <Builder/Builder.h>
#include <evmc/evmc.h>

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

bool TestcaseLoader::load(std::string _base, std::ostream &derr)
{
    FileIterator fit;
    fit.visit(_base, [&](fs::path file){
        if( file.has_extension() )
        {
            const std::string ext = file.extension().string();
            const std::string full_name = file.string();

            if( ext != ".json" )
                return ;

            std::string json_str = GetFileContent(full_name);
            nlohmann::json json;
            Testcase test;

            json = nlohmann::json::parse(json_str.begin(), json_str.end());

            test.name        = json["name"];
            test.source_path = json["source"];
            test.json_path   = full_name;
            test.input       = hex2Uint8Vec(json["input"]);
            test.expect      = hex2Uint8Vec(json["expect"]);
            test.binary      = {};

            if( json.count("contract_name") )
            {
                test.contract_name = json["contract_name"];
            }

            if( json.count("expect_code") )
            {
                test.expect_code = evmc_status_code((int)json["expect_code"]);
            }
            else
            {
                test.expect_code = EVMC_SUCCESS;
            }

            if( json.count("expect_log") )
            {
                for(const auto &lg:json["expect_log"])
                {
                    test.log.emplace_back();

                    assert(lg.count("addr"));
                    assert(lg.count("topics"));
                    assert(lg.count("data"));

                    memcpy(test.log.back().addr.bytes, hex2Uint8Vec(lg["addr"]).data(), sizeof(test.log.back().addr.bytes));
                    for(const auto &t:lg["topics"])
                    {
                        test.log.back().topics.emplace_back();
                        memcpy(test.log.back().topics.back().bytes, hex2Uint8Vec(t).data(), sizeof(test.log.back().topics.back().bytes));
                    }
                    test.log.back().data = hex2Uint8Vec(lg["data"]);
                }
            }
            
            fs::path source_path = test.source_path;
            if(source_path.is_relative()){
                source_path = fs::proximate(file.remove_filename()) /source_path;
            }
            source_path = fs::absolute(source_path);
            test.source_path = source_path.string();

            if( !fs::is_regular_file(test.source_path) )
            {
                derr << file << " Ignored!" << std::endl;
                derr << "   Can not open source : " << test.source_path << std::endl;
                return ;
            }

            if( !source_path.has_extension() )
            {
                derr << file << " Ignored!" << std::endl;
                derr << "   No Builder : " << test.source_path << std::endl;
                return ;
            }

            std::string sext = source_path.extension().string();
            bool isbuilt = false;
            for(Builder *ptr:m_builder)
            {
                if( contain( ptr->acceptExtensions(), sext ) )
                {
                    if( ptr->build(source_path, test.contract_name) )
                    {
                        test.binary = hex2Uint8Vec(ptr->getBinary());
                        test.state = TestcaseState::Ready;
                    }
                    else
                    {
                        test.state = TestcaseState::CompileFail;
                        derr << file << " Ignored!" << std::endl;
                        derr<<"    Build "<< full_name <<" with " << ptr->getClassName() << " FAIL!" <<std::endl;
                    }
                    isbuilt = true;
                    break;
                }
            }

            if( !isbuilt )
            {
                test.state = TestcaseState::NoMatchedBuilder;

                derr << file << " Ignored!" << std::endl;
                derr<<"    No Builder for "<< full_name << std::endl;
            }
            m_testcases.emplace_back(test);
        }
    });
    return true;
}

const std::vector<Testcase> &TestcaseLoader::testcases()
{
    return m_testcases;
}
