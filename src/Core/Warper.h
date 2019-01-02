#include <Third/EVMC/include/evmc/loader.h>
#include <Third/EVMC/include/evmc/evmc.h>
#include <string>

#include <ostream>

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
};