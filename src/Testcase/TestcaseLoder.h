#include<algorithm>
#include<string>
#include<filesystem>
#include<fstream>
#include<functional>
#include<string>
#include<iostream>

#include<Common/Algorithm.h>
#include<Common/FileUtils.h>
#include<Filesystem/FileIterator.h>
#include<Testcase/Testcase.h>
#include<Builder/Builder.h>

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
        m_builder.emplace_back(new LityBuilder);
        m_builder.emplace_back(new SoldityBuilder);
    }

    ~TestcaseLoader()
    {
        for(Builder *ptr:m_builder)
            delete ptr;
    }

    void addBuilder(Builder *_builder)
    {
        m_builder.emplace_back(_builder);
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

                for(Builder *ptr:m_builder)
                {
                    if( contain( ptr->acceptExtensions(), ext ) )
                    {
                        std::string data = GetFileContent(full_name);
                        if( ptr->build(data) )
                        {
                            m_testcases.emplace_back( ptr->getTestcase() );
                            std::cout<<"Build "<<full_name<<" with "<<ptr->getClassName()<<" SUCCESS!"<<std::endl;
                            std::cout<<"=========\n"<<m_testcases.back().m_binary<<"\n===============\n"<<std::endl;
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
};
