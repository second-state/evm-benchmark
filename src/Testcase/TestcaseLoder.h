#pragma once

#include<Testcase/Testcase.h>
#include<Builder/Builder.h>

#include<string>

class TestcaseLoader
{
public:
    TestcaseLoader();
    ~TestcaseLoader();
    void addBuilder(Builder *_builder);
    void clear();
    void load(std::string _base);
    const std::vector<Testcase> &testcases();
private:
    std::vector<Testcase> m_testcases;
    std::vector<Builder*> m_builder;
};
