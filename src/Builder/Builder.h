#pragma once

#include <Common/FileUtils.h>
#include <Testcase/Testcase.h>

#include <cstdlib>
#include <string>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

class Builder
{
protected:
    std::string getTmpDir();
    std::string callMakefile();

    static std::string getScriptExtenstion()
    {
#ifdef _WIN32
        return ".bat";
#else
        return ".sh";
#endif
    }

    bool callScript(fs::path source_path, std::string contract_name)
    {
        fs::path oldCurrentPath(fs::current_path());
        fs::path workPath("./temp");

        std::string scriptPath = oldCurrentPath.string() + "/scripts/" + getClassName() + getScriptExtenstion();

        if( !fs::exists(workPath) )
            fs::create_directory(workPath);
        
        if( !fs::is_directory(workPath) )
            return false;

        bool result = false;
        std::string OutputFilename =  contract_name + ".bin-runtime";
        do{
            fs::current_path(workPath);

            //TODO: Rewrite this!
            std::string executeCommand = scriptPath + " \"" + source_path.string() + "\"";
            [[maybe_unused]] int sys = system(executeCommand.c_str());

            if( testScriptRunSuccess(OutputFilename) )
                result = true;
        }while(false);

        fs::current_path(oldCurrentPath);
        fs::remove_all(workPath);
        return result;
    }
protected:
    std::string m_binary;
    virtual bool testScriptRunSuccess(std::string tmpOutputFilename)
    {
        m_binary.clear();
        if( fs::exists(tmpOutputFilename) )
        {
            m_binary = GetFileContent(tmpOutputFilename);
            return true;
        }
        return false;
    };
public:
    virtual std::string getClassName() = 0;
	virtual std::string getMakefileName() { assert(false); return ""; };
    virtual bool build(fs::path source_path, std::string contract_name) = 0;
    virtual std::vector<std::string> acceptExtensions() = 0;
    virtual std::string getBinary(){ return m_binary; }
    Builder(){};
};

class BinaryBuilder: public Builder
{
public:
    virtual std::string getClassName() override
    {
        return "BinaryBuilder";
    }

    virtual std::vector<std::string> acceptExtensions() override
    {
        return {".bin"};
    }

    virtual bool build(fs::path source_path, std::string contract_name) override
    {
        m_binary = GetFileContent(source_path.string());
        return true;
    }
};

class LityBuilder: public Builder
{
public:
    virtual std::string getClassName() override
    {
        return "LityBuilder";
    }

    virtual std::vector<std::string> acceptExtensions() override
    {
        return {".lity"};
    }

    virtual bool build(fs::path source_path, std::string contract_name) override
    {
        m_binary = "";
        if( !callScript(source_path, contract_name) )
        {
            return false;
        }
        return true;
    }
};

class SoldityBuilder: public Builder
{
public:
    virtual std::string getClassName() override
    {
        return "SoldityBuilder";
    }

    virtual std::vector<std::string> acceptExtensions() override
    {
        return {".sol"};
    }

    virtual bool build(fs::path source_path, std::string contract_name) override
    {
        m_binary = "";
        if( !callScript(source_path, contract_name) )
        {
            return false;
        }
        return true;
    }
};
