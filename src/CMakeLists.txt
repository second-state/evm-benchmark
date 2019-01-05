cmake_minimum_required (VERSION 3.8)

set(MAIN_FILE main.cpp)
set(MAIN_EXEC aaaa)
add_executable(${MAIN_EXEC} ${MAIN_FILE})

add_subdirectory(Builder)
#add_subdirectory(Common)
add_subdirectory(Core)
add_subdirectory(Filesystem)
add_subdirectory(Testcase)
add_subdirectory(Third/EVMC)


target_link_libraries(${MAIN_EXEC} loader instructions)
target_link_libraries(${MAIN_EXEC} Testcase Core)
if (NOT MSVC)
    target_link_libraries(${MAIN_EXEC} stdc++fs)
endif()
