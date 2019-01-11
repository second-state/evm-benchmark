#include <Core/Content.h>
#include <Third/EVMC/include/evmc/helpers.h>
#include <Third/EVMC/include/evmc/loader.h>
#include <Third/EVMC/include/evmc/evmc.h>

#include <cstring>
#include <string>
#include <ostream>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;

class VMInterface
{
    std::string m_vmpath;
    evmc_create_fn m_vmcreater;
    evmc_loader_error_code m_error;

    evmc_instance* getNewEVMCInstance()
    {
        if( !m_vmcreater )
            return nullptr;
        return m_vmcreater();
    }

public:

    VMInterface(std::string _vmpath):m_vmpath(_vmpath)
    {
        m_vmcreater = evmc_load(m_vmpath.c_str(), &m_error);
    }

    evmc_loader_error_code getLoadStatus()
    {
        return m_error;
    }

    bool isOpen()
    {
        return m_error == EVMC_LOADER_SUCCESS;
    }

    void showVMInformation(std::ostream &out)
    {
        evmc_instance* instance = getNewEVMCInstance();

        out << "VM [" << m_vmpath << "]" << std::endl;
        if( !isOpen() || !instance )
        {
            out << "Load Failed! (" << m_error << ")" << std::endl;
            return ;
        }

        out << "ABI    : " << instance->abi_version << std::endl;
        out << "Name   : " << instance->name << std::endl;
        out << "Version: " << instance->version << std::endl;
    }

    static void free_result_output_data(const struct evmc_result* result)
    {
        free((uint8_t*)result->output_data);
    }

    evmc_result execute(const std::vector<uint8_t> &opcode, const std::vector<uint8_t> &input)
    {
        evmc_instance* instance =  getNewEVMCInstance();
        evmc_context *context = EVMCContent::getNewContents();

        //TODO: move evmc_message instead of input
        evmc_message msg;
        const evmc_address addr = {{0, 1, 2}};
        const evmc_uint256be value = {{1, 0}};

        msg.sender = addr;
        msg.destination = addr;
        msg.value = value;
        msg.input_data = input.data();
        msg.input_size = input.size();
        msg.gas = 7120000;
        msg.depth = 20;

        evmc_execute_fn exec = instance->execute;
        struct evmc_result result = exec(instance, context, EVMC_HOMESTEAD, &msg, opcode.data(), opcode.size());

        cout<<"S="<<result.status_code<<endl;
        cout<<"G="<<result.gas_left<<endl;
        cout<<"O="<<result.output_size<<endl;
        for(int i=0;i<result.output_size;++i)
        {
            cout<< std::hex << (int) result.output_data[i] << ' '<<endl; 
        }

        return result;
    }
};