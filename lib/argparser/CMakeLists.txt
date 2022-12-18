add_library(cmdargs argparser.cpp)

target_link_libraries(cmdargs PRIVATE liblogs)

target_include_directories(cmdargs PUBLIC
                    ${CMAKE_CURRENT_LIST_DIR})