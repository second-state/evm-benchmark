#pragma once

#include <evmc/evmc.h>
#include <evmc/helpers.hpp>
#include <evmc/loader.h>
#include <map>
#include <iostream>
#include <vector>

struct VirtualEVMCContent : evmc_context
{
    struct account
    {
        evmc_uint256be balance = {};
        size_t code_size = 0;
        evmc_bytes32 code_hash = {};
        std::map<evmc_bytes32, evmc_bytes32> storage;
    };

    evmc_tx_context tx_context = {};
    std::map<evmc_address, account> accounts;
    evmc_instance* instance;

    struct vmlog
    {
        evmc_address addr;
        std::vector<evmc_bytes32> topics;
        std::vector<uint8_t> data;
        bool operator==(const vmlog &b) const
        {
            if( memcmp(addr.bytes, b.addr.bytes, sizeof(addr.bytes)) )
                return false;
            if( data!=b.data )
                return false;
            if( topics.size()!=b.topics.size() )
                return false;
            for(size_t i=0; i<topics.size(); ++i)
            {
                if( memcmp(topics[i].bytes, b.topics[i].bytes, sizeof(topics[i].bytes)) )
                    return false;
            }
            return true;
        }
    };
    std::vector<vmlog> log;

    std::vector<uint8_t> _opcode; //TODO: Remove this

    void clear()
    {
        accounts.clear();
        log.clear();
        instance = nullptr;
    }

    void exportStateDetail()
    {
        //TODO
    }
};

class EVMCContent
{
    static bool account_exists(evmc_context* context, const evmc_address* address)
    {
        VirtualEVMCContent* host = reinterpret_cast<VirtualEVMCContent*>(context);
        return host->accounts.find(*address) != host->accounts.end();
    }

    static evmc_bytes32 get_storage(evmc_context* context, const evmc_address* address, const evmc_bytes32* key)
    {
        VirtualEVMCContent* host = reinterpret_cast<VirtualEVMCContent*>(context);
        auto it = host->accounts.find(*address);
        if (it != host->accounts.end())
            return it->second.storage[*key];
        return {};
    }

    static enum evmc_storage_status set_storage(evmc_context* context, const evmc_address* address, const evmc_bytes32* key, const evmc_bytes32* value)
    {
        VirtualEVMCContent* host = reinterpret_cast<VirtualEVMCContent*>(context);
        auto& account = host->accounts[*address];
        auto prevValue = account.storage[*key];
        account.storage[*key] = *value;

        if (prevValue == *value)
            return EVMC_STORAGE_UNCHANGED;
        else
            return EVMC_STORAGE_MODIFIED;
    }

    static evmc_uint256be get_balance(evmc_context* context, const evmc_address* address)
    {
        VirtualEVMCContent* host = reinterpret_cast<VirtualEVMCContent*>(context);
        auto it = host->accounts.find(*address);
        if (it != host->accounts.end())
            return it->second.balance;
        return {};
    }

    static size_t get_code_size(evmc_context* context, const evmc_address* address)
    {
        VirtualEVMCContent* host = reinterpret_cast<VirtualEVMCContent*>(context);
        auto it = host->accounts.find(*address);
        if (it != host->accounts.end())
            return it->second.code_size;
        return 0;
    }

    static evmc_bytes32 get_code_hash(evmc_context* context, const evmc_address* address)
    {
        VirtualEVMCContent* host = reinterpret_cast<VirtualEVMCContent*>(context);
        auto it = host->accounts.find(*address);
        if (it != host->accounts.end())
            return it->second.code_hash;
        return {};
    }

    static size_t copy_code(evmc_context* context, const evmc_address* address, size_t code_offset, uint8_t* buffer_data, size_t buffer_size)
    {
        (void)context;
        (void)address;
        (void)code_offset;
        (void)buffer_data;
        (void)buffer_size;
        return 0;
    }

    static void selfdestruct(evmc_context* context,
                            const evmc_address* address,
                            const evmc_address* beneficiary)
    {
        (void)context;
        (void)address;
        (void)beneficiary;
    }
// https://github.com/ethereum/aleth/blob/master/libethereum/ExtVM.cpp#L154
// https://github.com/ethereum/aleth/blob/master/libevm/ExtVMFace.cpp#L186
// https://github.com/ethereum/aleth/blob/master/libethereum/Executive.cpp#L357
    static evmc_result create(evmc_context* context, const evmc_message* msg)
    {
        // Unimp EVMC_CREATE/EVMC_CREATE2
        evmc_result result{};

        result.status_code = EVMC_FAILURE;

        return result;
    }

    static evmc_result call(evmc_context* context, const evmc_message* msg)
    {
        VirtualEVMCContent* host = reinterpret_cast<VirtualEVMCContent*>(context);
        evmc_result result{};

        result.status_code = EVMC_FAILURE;

        if( msg->kind == EVMC_CREATE || msg->kind == EVMC_CREATE2 )
        {
            return create(context, msg); 
        }

        evmc_execute_fn exec = host->instance->execute;
        result = exec(host->instance, context, EVMC_MAX_REVISION, msg, host->_opcode.data(), host->_opcode.size());
        
        return result;
    }

    static evmc_tx_context get_tx_context(evmc_context* context)
    {
        (void)context;
        evmc_tx_context result{};
        return result;
    }

    static evmc_bytes32 get_block_hash(evmc_context* context, int64_t number)
    {
        VirtualEVMCContent* host = reinterpret_cast<VirtualEVMCContent*>(context);
        int64_t current_block_number = host->tx_context.block_number;

        evmc_bytes32 example_block_hash;
        if (number < current_block_number && number >= current_block_number - 256)
            example_block_hash = {{1, 1, 1, 1}};
        else
            example_block_hash = {};
        return example_block_hash;
    }

    static void emit_log(evmc_context* context, const evmc_address* address, const uint8_t* data, size_t data_size, const evmc_bytes32 topics[], size_t topics_count)
    {
        VirtualEVMCContent* host = reinterpret_cast<VirtualEVMCContent*>(context);

        VirtualEVMCContent::vmlog temp;
        temp.addr = *address;
        temp.data.assign(data, data+data_size);
        temp.topics.assign(topics, topics+topics_count);

        host->log.emplace_back(std::move(temp));
    }
public:
    static evmc_context* getNewContents(evmc_instance* instance)
    {
        static evmc_context* pcontent = nullptr;
        if( pcontent == nullptr )
        {
            evmc_host_interface *host = new evmc_host_interface;
            VirtualEVMCContent *content = new VirtualEVMCContent;

            host->account_exists    = EVMCContent::account_exists;
            host->get_storage       = EVMCContent::get_storage;
            host->set_storage       = EVMCContent::set_storage;
            host->get_balance       = EVMCContent::get_balance;
            host->get_code_size     = EVMCContent::get_code_size;
            host->get_code_hash     = EVMCContent::get_code_hash;
            host->copy_code         = EVMCContent::copy_code;
            host->selfdestruct      = EVMCContent::selfdestruct;
            host->call              = EVMCContent::call;
            host->get_tx_context    = EVMCContent::get_tx_context;
            host->get_block_hash    = EVMCContent::get_block_hash;
            host->emit_log          = EVMCContent::emit_log;

            content->host = host;
            pcontent = (evmc_context*)content;
        }
        reinterpret_cast<VirtualEVMCContent*>(pcontent)->clear();
        reinterpret_cast<VirtualEVMCContent*>(pcontent)->instance = instance;
        return pcontent;
    }
};