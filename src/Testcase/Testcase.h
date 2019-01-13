#pragma once

#include <string>
#include <vector>

struct Testcase
{
    std::string name;
    std::string source_path;
    std::string json_path;

    std::vector<uint8_t> input;
    std::vector<uint8_t> expect;
    std::vector<uint8_t> binary;
};