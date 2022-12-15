add_library(parser parser.cpp)

target_link_libraries(parser PUBLIC ast lexer liblogs)

target_include_directories(parser PUBLIC
                        ${CMAKE_CURRENT_LIST_DIR})