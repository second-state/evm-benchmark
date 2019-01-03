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

    evmc_result execute(const std::vector<uint8_t> &opcode, const std::vector<uint8_t> &input)
    {

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