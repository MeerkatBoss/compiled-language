add_library(compiler compiler.cpp)

target_link_libraries(compiler PUBLIC liblogs name_scopes ast)

target_include_directories(compiler PUBLIC
                        ${CMAKE_CURRENT_LIST_DIR})