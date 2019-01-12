#pragma once

#include<Testcase/Testcase.h>
#include<Builder/Builder.h>

#include<string>

class TestcaseLoader
{
public:
    TestcaseLoader(std::string _base);
    ~TestcaseLoader();
    void addBuilder(Builder *_builder);
    void clear();
    void load();
    const std::vector<Testcase> &testcases();
private:
    std::string m_base;
    std::vector<Testcase> m_testcases;
    std::vector<Builder*> m_builder;
};
