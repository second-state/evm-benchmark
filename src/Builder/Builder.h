#pragma once

#include<string>
#include<cassert>
#include<vector>

#include<Testcase/Testcase.h>

class Builder
{
protected:
    std::string getTmpDir();
    std::string callMakefile();
public:
    virtual std::string getMakefileName() { assert(false); };
    virtual Testcase build(std::string source_path) = 0;
    virtual std::vector<std::string> acceptExtensions() = 0;
    Builder(){};
};

class BinaryBuilder: public Builder
{
public:
    virtual std::vector<std::string> acceptExtensions() override
    {
        return {".bin"};
    }

    virtual Testcase build(std::string source_path) override
    {
        Testcase tmp;
        return tmp;
    }
};
