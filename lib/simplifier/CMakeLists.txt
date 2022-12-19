add_library(simplifier simplifier.cpp)

target_link_libraries(simplifier PUBLIC liblogs ast)

target_include_directories(simplifier PUBLIC
                        ${CMAKE_CURRENT_LIST_DIR})