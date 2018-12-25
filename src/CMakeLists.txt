cmake_minimum_required (VERSION 3.8)

set(TEST_EXEC main.cpp)
add_executable(aaaa ${TEST_EXEC})
add_subdirectory(Third/EVMC)

if (NOT MSVC)
    target_link_libraries(aaaa stdc++fs)
endif()
