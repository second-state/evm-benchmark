#pragma once

#include <string>
#include <vector>
#include <evmc/evmc.h>

#include <Core/Content.h>

enum class TestcaseState
{
    Ready,
    CompileFail,
    NoMatchedBuilder,
    UnknownError
};

struct Testcase
{
    TestcaseState state = TestcaseState::UnknownError;

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