add_executable(mbc_back main.cpp back_flags.cpp back_utils.cpp)

target_link_libraries(mbc_back PUBLIC liblogs cmdargs ast compiler)

target_include_directories(mbc_back PRIVATE ${CMAKE_CURRENT_LIST_DIR})

add_run_target(mbc_back)