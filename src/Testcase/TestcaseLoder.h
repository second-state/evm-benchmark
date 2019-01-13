#pragma once

#include <Testcase/Testcase.h>
#include <Builder/Builder.h>

#include <ostream>
#include <string>

class TestcaseLoader
{
public:
    TestcaseLoader();
    ~TestcaseLoader();
    void addBuilder(Builder *_builder);
    void clear();
    bool load(std::string _base, std::ostream &derr);
    const std::vector<Testcase> &testcases();
private:
    std::vector<Testcase> m_testcases;
    std::vector<Builder*> m_builder;
};
