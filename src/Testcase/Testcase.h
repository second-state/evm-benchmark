#pragma once

#include <string>
#include <vector>
#include <evmc/evmc.h>

#include <Core/Content.h>

struct Testcase
{
    std::string name;
    std::string source_path;
    std::string contract_name;
    std::string json_path;

    evmc_status_code expect_code;

    std::vector<uint8_t> input;
    std::vector<uint8_t> expect;
    std::vector<uint8_t> binary;
    std::vector<VirtualEVMCContent::vmlog> log;
};