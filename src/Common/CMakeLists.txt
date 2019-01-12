add_library(Common STATIC
    Algorithm.cpp
    Algorithm.h
    FileUtils.cpp
    FileUtils.h
)
set_target_properties(Common PROPERTIES LINKER_LANGUAGE CXX)
