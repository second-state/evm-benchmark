#include <Core/Warper.h>
#include <Core/Content.h>
#include <Testcase/Testcase.h>
#include <Common/Algorithm.h>

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

evmc_result VMInterface::execute(const Testcase &testcase, evmc_message &msg, std::chrono::nanoseconds &runtime, evmc_context *&context)
{
    msg = {};

    auto address = hex2Uint8Vec(testcase.json.back()["exec"]["address"]);
    memcpy(msg.destination.bytes,address.data(),address.size()*sizeof(uint8_t));

    auto caller = hex2Uint8Vec(testcase.json.back()["exec"]["caller"]);
    memcpy(msg.sender.bytes,caller.data(),caller.size()*sizeof(uint8_t));

    auto value = hex2Uint8Vec(testcase.json.back()["exec"]["value"]);
    memcpy(msg.value.bytes,value.data(),value.size()*sizeof(uint8_t));

    msg.input_data = testcase.data.data();
    msg.input_size = testcase.data.size();

    if(testcase.json.back()["exec"].count("gas"))
        msg.gas = hex2int64(testcase.json.back()["exec"]["gas"]);
    else
        msg.gas = 1000000000;
    msg.depth = 0;

    evmc_instance* instance =  getNewEVMCInstance();
    context = EVMCContent::getNewContents(instance);

    evmc_tx_context &tx_context = reinterpret_cast<VirtualEVMCContent*>(context)->tx_context;

    if(testcase.json.back()["exec"].count("gasPrice")){
        auto gasPrice = hex2Uint8Vec(testcase.json.back()["exec"]["gasPrice"]);
        memcpy(tx_context.tx_gas_price.bytes,gasPrice.data(),gasPrice.size()*sizeof(uint8_t));
    }

    if(testcase.json.back()["exec"].count("origin")){
        auto origin = hex2Uint8Vec(testcase.json.back()["exec"]["origin"]);
        memcpy(tx_context.tx_origin.bytes,origin.data(),origin.size()*sizeof(uint8_t));
    }

    if(testcase.json.back().count("env")){
        auto coinbase = hex2Uint8Vec(testcase.json.back()["env"]["currentCoinbase"]);
        memcpy(tx_context.block_coinbase.bytes,coinbase.data(),coinbase.size()*sizeof(uint8_t));
        auto difficulty = hex2Uint8Vec(testcase.json.back()["env"]["currentDifficulty"]);
        memcpy(tx_context.block_difficulty.bytes,difficulty.data(),difficulty.size()*sizeof(uint8_t));

        tx_context.block_gas_limit = hex2int64(testcase.json.back()["env"]["currentGasLimit"]);
        tx_context.block_number = hex2int64(testcase.json.back()["env"]["currentNumber"]);
        tx_context.block_timestamp = hex2int64(testcase.json.back()["env"]["currentTimestamp"]);
    }

    auto &base = reinterpret_cast<VirtualEVMCContent*>(context)->accounts[msg.sender];
    reinterpret_cast<VirtualEVMCContent*>(context)->_opcode = testcase.binary;
    base.code_size = testcase.binary.size();

    evmc_execute_fn exec = instance->execute;

    auto t_start = std::chrono::high_resolution_clock::now();
    evmc_result result = exec(instance, context, EVMC_MAX_REVISION, &msg, testcase.binary.data(), testcase.binary.size());
    auto t_end   = std::chrono::high_resolution_clock::now();

    runtime = std::chrono::duration_cast<std::chrono::nanoseconds>(t_end - t_start);
    return result;
}
