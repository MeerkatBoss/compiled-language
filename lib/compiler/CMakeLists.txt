add_library(compiler compiler.cpp size_array.cpp)

target_link_libraries(compiler PUBLIC liblogs name_scopes ast dynamicarray)

target_include_directories(compiler PUBLIC
                        ${CMAKE_CURRENT_LIST_DIR})