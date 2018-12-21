#pragma once

#include<string>

class Testcase
{
public:
    Testcase(std::string _hexopcode):m_binary(_hexopcode){}

    std::string m_binary;
    std::string m_path;
};