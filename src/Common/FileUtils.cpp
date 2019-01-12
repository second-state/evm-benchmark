#include <Common/FileUtils.h>

#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

std::string GetFileContent(std::string path)
{
    std::ifstream fin(path);
    std::string result;

    if( fin )
    {
        result = std::string(std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>());
    }

    return result;
}