add_executable(mbc-back main.cpp)

target_link_libraries(mbc-back PUBLIC liblogs)

target_include_directories(mbc-back PRIVATE ${CMAKE_CURRENT_LIST_DIR})