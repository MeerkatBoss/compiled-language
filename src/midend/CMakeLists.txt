add_executable(mbc_mid main.cpp)

target_link_libraries(mbc_mid PUBLIC liblogs)

target_include_directories(mbc_mid PRIVATE ${CMAKE_CURRENT_LIST_DIR})

add_run_target(mbc_mid)