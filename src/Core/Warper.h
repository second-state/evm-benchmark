#include <Third/EVMC/include/evmc/helpers.h>
#include <Third/EVMC/include/evmc/loader.h>
#include <Third/EVMC/include/evmc/evmc.h>

#include <cstring>
#include <string>
#include <ostream>
#include <iostream>
#include <vector>

class VMInterface
{
    std::string m_vmpath;
    evmc_instance* m_instance;
    evmc_loader_error_code m_error;
public:
    VMInterface(std::string _vmpath):m_vmpath(_vmpath)
    {
        m_instance = evmc_load_and_create(m_vmpath.c_str(), &m_error);
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
        out << "VM [" << m_vmpath << "]" << std::endl;
        if( !isOpen() )
        {
            out << "Load Failed! (" << m_error << ")" << std::endl;
            return ;
        }

        out << "ABI    : " << m_instance->abi_version << std::endl;
        out << "Name   : " << m_instance->name << std::endl;
        out << "Version: " << m_instance->version << std::endl;
    }

    static void free_result_output_data(const struct evmc_result* result)
    {
        free((uint8_t*)result->output_data);
    }

    evmc_result execute(const std::vector<uint8_t> &opcode, const std::vector<uint8_t> &input)
    {
        evmc_host_interface ehost;
        evmc_context econtext{&ehost}, *context = &econtext;
        evmc_message emsg, *msg = &emsg;
        enum evmc_revision rev = EVMC_LATEST_REVISION;

        msg->kind = EVMC_CALL;
        msg->flags = 0;
        msg->depth = 20;
        msg->gas   = 71222; 
        msg->input_data = input.data();
        msg->input_size = input.size();

        struct evmc_result ret = {.status_code = EVMC_INTERNAL_ERROR};
        if (opcode.empty())
        {
            // In case of empty code return a fancy error message.
            const char* error = rev == EVMC_BYZANTIUM ? "Welcome to Byzantium!" : "Hello Ethereum!";
            ret.output_data = (const uint8_t*)error;
            ret.output_size = strlen(error);
            ret.status_code = EVMC_FAILURE;
            ret.release = NULL;  // We don't need to release the constant messages.
            return ret;
        }
        
        // Simulate executing by checking for some code patterns.
        // Solidity inline assembly is used in the examples instead of EVM bytecode.

        // Assembly: `{ mstore(0, address()) return(0, msize()) }`.
        const char return_address[] = "\x30\x60\x00\x52\x59\x60\x00\xf3";

        // Assembly: `{ sstore(0, add(sload(0), 1)) }`
        const char counter[] = "\x60\x01\x60\x00\x54\x01\x60\x00\x55";

        if (opcode.size() == strlen(return_address) &&
            strncmp((const char*)opcode.data(), return_address, strlen(return_address)) == 0)
        {
            static const size_t address_size = sizeof(msg->destination);
            uint8_t* output_data = (uint8_t*)malloc(address_size);
            if (!output_data)
            {
                // malloc failed, report internal error.
                ret.status_code = EVMC_INTERNAL_ERROR;
                return ret;
            }
            memcpy(output_data, &msg->destination, address_size);
            ret.status_code = EVMC_SUCCESS;
            ret.output_data = output_data;
            ret.output_size = address_size;
            ret.release = &free_result_output_data;
            return ret;
        }
        else if (opcode.size() == strlen(counter) && strncmp((const char*)opcode.data(), counter, strlen(counter)) == 0)
        {
            const evmc_bytes32 key = {{0}};
            evmc_bytes32 value = context->host->get_storage(context, &msg->destination, &key);
            value.bytes[31]++;
            context->host->set_storage(context, &msg->destination, &key, &value);
            ret.status_code = EVMC_SUCCESS;
            return ret;
        }

        ret.status_code = EVMC_FAILURE;
        ret.gas_left = 0;

        return ret;
    }

    void test()
    {
        evmc_execute_fn exec = m_instance->execute;

        evmc_host_interface ehost;
        evmc_context econt {&ehost};
        evmc_message emsg;

        evmc_get_balance_fn ebn { [](struct evmc_context* context,  const evmc_address* address)->evmc_uint256be{
            std::cout<<"test ebn!"<<std::endl;
            evmc_bytes32 test;
            for( int i=0 ; i<32 ; ++i )
            {
                test.bytes[i] = i;
            }
            return test;
        } };

        memset(&ehost, 0, sizeof(ehost));
        ehost.get_balance = ebn;

        emsg.kind = EVMC_CALL;
        emsg.flags = 0;
        emsg.depth = 20;
        emsg.gas   = 71222; 
        emsg.input_data = NULL;
        emsg.input_size = 0;

        uint8_t opcode[] = {0x42, 0x31};
        evmc_result res = exec(m_instance, &econt, EVMC_LATEST_REVISION, &emsg, opcode, sizeof(opcode));

        std::cout<<"status:"<<res.status_code<<std::endl;
        std::cout<<"gas   :"<<res.gas_left   <<std::endl;
    }
};