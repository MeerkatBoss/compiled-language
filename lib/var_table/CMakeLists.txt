add_library(var_table var_table.cpp table_stack.cpp)

target_link_libraries(var_table PUBLIC liblogs dynamicarray)

target_include_directories(var_table PUBLIC
                        ${CMAKE_CURRENT_LIST_DIR})