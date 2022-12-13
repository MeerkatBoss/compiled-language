add_library(lexer lexer_tree.cpp token_list.cpp lexer.cpp)

target_link_libraries(lexer PUBLIC liblogs dynamicarray libinput)

target_include_directories(lexer PUBLIC
                        ${CMAKE_CURRENT_LIST_DIR})