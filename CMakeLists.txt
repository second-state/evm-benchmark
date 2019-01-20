cmake_minimum_required (VERSION 3.8)

project(ejudge CXX)
set (CMAKE_CXX_STANDARD 17)

include_directories(src)
include_directories(src/Third/nlohmann_json/include)

add_subdirectory(src)
