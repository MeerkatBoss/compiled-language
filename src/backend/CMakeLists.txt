add_executable(mbc_back main.cpp)

target_link_libraries(mbc_back PUBLIC liblogs)

target_include_directories(mbc_back PRIVATE ${CMAKE_CURRENT_LIST_DIR})

add_run_target(mbc_back)