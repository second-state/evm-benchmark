#include<string>
#include<filesystem>
#include<functional>
#include<string>
#include<iostream>

#include<Common/Algorithm.h>
#include<Filesystem/Fileiterator.h>
#include<Testcase/Testcase.h>
#include<Builder/builder.h>

namespace fs = std::filesystem;

class TestcaseLoader
{
    std::string m_base;
    std::vector<Testcase> m_testcases;
    std::vector<Builder*> m_builder;
public:
    TestcaseLoader(std::string _base):m_base(_base)
    {
        m_builder.emplace_back(new BinaryBuilder);
    }

    ~TestcaseLoader()
    {
        for(Builder *ptr:m_builder)
            delete ptr;
    }

    void clear()
    {
        m_testcases.clear();
    }

    void load()
    {
        FileIterator fit;
        fit.visit(m_base, [&](fs::path file){
            if( file.has_extension() )
            {
                const std::string ext = file.extension().string();
                const std::string full_name = file.string();
                std::cout<< ext <<'\t' << full_name << std::endl;

                for(Builder *ptr:m_builder)
                {
                    if( contain( ptr->acceptExtensions(), ext ) )
                    {
                        std::cout<<"Accepted"<<std::endl;
                        m_testcases.emplace_back( ptr->build(full_name) );
                    }
                }
            }
        });
    }
};
