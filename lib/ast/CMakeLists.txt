add_library(ast ast.cpp ast_dsl.cpp)

target_link_libraries(ast PUBLIC liblogs dynamicarray mathutils)

target_include_directories(ast PUBLIC
                        ${CMAKE_CURRENT_LIST_DIR})