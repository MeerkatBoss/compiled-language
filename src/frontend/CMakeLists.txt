add_executable(mbc_front main.cpp)

target_link_libraries(mbc_front PUBLIC liblogs lexer ast)

target_include_directories(mbc_front PRIVATE ${CMAKE_CURRENT_LIST_DIR})

add_run_target(mbc_front)