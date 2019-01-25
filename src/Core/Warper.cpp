#include <Core/Warper.h>

#include <Core/Content.h>
#include <evmc/helpers.h>
#include <evmc/loader.h>
#include <evmc/evmc.h>

#include <ctime>
#include <chrono>
#include <cstring>
#include <string>
#include <ostream>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;

evmc_instance* VMInterface::getNewEVMCInstance()
{
    if( !m_vmcreater )
        return nullptr;
    return m_vmcreater();
}

VMInterface::VMInterface(std::string _vmpath):m_vmpath(_vmpath)
{
    m_vmcreater = evmc_load(m_vmpath.c_str(), &m_error);
}

evmc_loader_error_code VMInterface::getLoadStatus()
{
    return m_error;
}

bool VMInterface::isOpen()
{
    return m_error == EVMC_LOADER_SUCCESS;
}

void VMInterface::showVMInformation(std::ostream &out)
{
    evmc_instance* instance = getNewEVMCInstance();

    out << "=== VM [" << m_vmpath << "]" << std::endl;
    if( !isOpen() || !instance )
    {
        out << "Load Failed! (" << m_error << ")" << std::endl;
        return ;
    }

    out << "ABI    : " << instance->abi_version << std::endl;
    out << "Name   : " << instance->name << std::endl;
    out << "Version: " << instance->version << std::endl;
}

evmc_result VMInterface::execute(const std::vector<uint8_t> &opcode, const evmc_message &msg, std::chrono::nanoseconds &runtime)
{
    evmc_instance* instance =  getNewEVMCInstance();
    evmc_context *context = EVMCContent::getNewContents();

    evmc_execute_fn exec = instance->execute;

    auto t_start = std::chrono::high_resolution_clock::now();
    evmc_result result = exec(instance, context, EVMC_LATEST_REVISION, &msg, opcode.data(), opcode.size());
    auto t_end   = std::chrono::high_resolution_clock::now();

    runtime = std::chrono::duration_cast<std::chrono::nanoseconds>(t_end - t_start);
    return result;
}
