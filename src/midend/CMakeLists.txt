add_executable(mbc-mid main.cpp)

target_link_libraries(mbc-mid PUBLIC liblogs)

target_include_directories(mbc-mid PRIVATE ${CMAKE_CURRENT_LIST_DIR})