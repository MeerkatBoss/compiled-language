add_executable(mbc_mid main.cpp mid_flags.cpp mid_utils.cpp)

target_link_libraries(mbc_mid PUBLIC liblogs cmdargs ast simplifier)

target_include_directories(mbc_mid PRIVATE ${CMAKE_CURRENT_LIST_DIR})

add_run_target(mbc_mid)