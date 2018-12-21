#include <Third/EVMC/include/evmc/loader.h>

#include<string>

class VMInterface
{
    std::string m_vmpath;
    evmc_create_fn m_vmcreat_func;
    evmc_loader_error_code m_error;
public:
    VMInterface(std::string _vmpath):m_vmpath(_vmpath)
    {
        m_vmcreat_func = evmc_load(m_vmpath.c_str(), &m_error);
    }

    evmc_loader_error_code getLoadStatus()
    {
        return m_error;
    }

    bool isOpen()
    {
        return m_error == EVMC_LOADER_SUCCESS;
    }
};