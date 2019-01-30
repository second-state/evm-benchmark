#pragma once

#include <Builder/Builder.h>
#include <Core/Warper.h>
#include <Testcase/TestcaseLoder.h>
#include <evmc/evmc.h>

#include <ostream>
#include <string>

class Benchmark
{
public:
    Benchmark(std::string _testsbase, std::string _vmpath, std::ostream &_out, std::ostream &_errout);

    void addBuilder(Builder *_builder);
    bool run();

private:
    bool prepare();
    bool genEnvInfo();
    bool runTests();

    void showOSInfo();

    std::ostream& dout();
    std::ostream& derr();

    TestcaseLoader m_casesloder;
    VMInterface m_vm;

    std::vector<std::string> m_builder_str;

    std::string m_testcasebase;
    std::string m_vmpath;
    std::ostream &m_stdout;
    std::ostream &m_stderr;

    const std::map<evmc_status_code, std::string> evmc_status_code_map = {
        {EVMC_SUCCESS, "EVMC_SUCCESS"},
        {EVMC_FAILURE, "EVMC_FAILURE"},
        {EVMC_REVERT, "EVMC_REVERT"},
        {EVMC_OUT_OF_GAS, "EVMC_OUT_OF_GAS"},
        {EVMC_INVALID_INSTRUCTION, "EVMC_INVALID_INSTRUCTION"},
        {EVMC_UNDEFINED_INSTRUCTION, "EVMC_UNDEFINED_INSTRUCTION"},
        {EVMC_STACK_OVERFLOW, "EVMC_STACK_OVERFLOW"},
        {EVMC_STACK_UNDERFLOW, "EVMC_STACK_UNDERFLOW"},
        {EVMC_BAD_JUMP_DESTINATION, "EVMC_BAD_JUMP_DESTINATION"},
        {EVMC_INVALID_MEMORY_ACCESS, "EVMC_INVALID_MEMORY_ACCESS"},
        {EVMC_CALL_DEPTH_EXCEEDED, "EVMC_CALL_DEPTH_EXCEEDED"},
        {EVMC_STATIC_MODE_VIOLATION, "EVMC_STATIC_MODE_VIOLATION"},
        {EVMC_PRECOMPILE_FAILURE, "EVMC_PRECOMPILE_FAILURE"},
        {EVMC_CONTRACT_VALIDATION_FAILURE, "EVMC_CONTRACT_VALIDATION_FAILURE"},
        {EVMC_ARGUMENT_OUT_OF_RANGE, "EVMC_ARGUMENT_OUT_OF_RANGE"},
        {EVMC_WASM_UNREACHABLE_INSTRUCTION, "EVMC_WASM_UNREACHABLE_INSTRUCTION"},
        {EVMC_WASM_TRAP, "EVMC_WASM_TRAP"},
        {EVMC_INTERNAL_ERROR, "EVMC_INTERNAL_ERROR"},
        {EVMC_REJECTED, "EVMC_REJECTED"}
    };
};
