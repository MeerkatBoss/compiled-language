add_executable(mbc-front main.cpp)

target_link_libraries(mbc-front PUBLIC liblogs)

target_include_directories(mbc-front PRIVATE ${CMAKE_CURRENT_LIST_DIR})