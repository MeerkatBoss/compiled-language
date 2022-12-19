add_executable(mbc_front main.cpp front_flags.cpp front_utils.cpp)

target_link_libraries(mbc_front PUBLIC libinput liblogs cmdargs lexer parser decompiler)

target_include_directories(mbc_front PRIVATE ${CMAKE_CURRENT_LIST_DIR})

add_run_target(mbc_front)