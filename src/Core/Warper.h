#pragma once

#include <Core/Content.h>
#include <evmc/helpers.h>
#include <evmc/loader.h>
#include <evmc/evmc.h>

#include <chrono>
#include <string>
#include <ostream>
#include <vector>

class VMInterface
{
    std::string m_vmpath;
    evmc_create_fn m_vmcreater;
    evmc_loader_error_code m_error;

    evmc_instance* getNewEVMCInstance();

public:

    VMInterface(std::string _vmpath);

    evmc_loader_error_code getLoadStatus();
    bool isOpen();
    void showVMInformation(std::ostream &out);

    static void free_result_output_data(const struct evmc_result* result)
    {
        free((uint8_t*)result->output_data);
    }

    evmc_result execute(const std::vector<uint8_t> &opcode, const evmc_message &msg, std::chrono::nanoseconds &runtime, evmc_context *&context);
};