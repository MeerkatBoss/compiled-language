add_library(decompiler decompiler.cpp)

target_link_libraries(decompiler PUBLIC liblogs ast lexer)

target_include_directories(decompiler PUBLIC
                        ${CMAKE_CURRENT_LIST_DIR})