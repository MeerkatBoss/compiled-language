add_library(name_scopes var_table.cpp table_stack.cpp func_array.cpp)

target_link_libraries(name_scopes PUBLIC liblogs dynamicarray ast)

target_include_directories(name_scopes PUBLIC
                        ${CMAKE_CURRENT_LIST_DIR})