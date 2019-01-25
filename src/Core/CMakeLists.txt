add_library(Core STATIC
    Content.h
    Warper.h
    Warper.cpp
    Benchmark.h
    Benchmark.cpp
)
target_link_libraries(Core evmc::loader)
set_target_properties(Core PROPERTIES LINKER_LANGUAGE CXX)
