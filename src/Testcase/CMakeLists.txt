add_library(Testcase STATIC
    Testcase.h
    TestcaseLoder.cpp
    TestcaseLoder.h
)
target_link_libraries(Testcase Common)
set_target_properties(Testcase PROPERTIES LINKER_LANGUAGE CXX)
