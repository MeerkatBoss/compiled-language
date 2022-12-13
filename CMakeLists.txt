cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(compiled_language VERSION 1.0)

include(run.cmake)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-D _DEBUG -ggdb3 -std=c++2a -O0 -Wall -Wextra\
    -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations\
    -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion\
    -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security\
    -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd\
    -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow\
    -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2\
    -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types\
    -Wsuggest-override -Wswitch-default -Wsync-nand -Wundef -Wunreachable-code\
    -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers\
    -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -Wswitch-enum\
    -fcheck-new -fsized-deallocation -fstack-check\
    -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -fPIE\
    -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr\
    -pie -Wlarger-than=8192 -Wstack-usage=8192") 

set(CMAKE_LINKER_FLAGS "${CMAKE_CXX_FLAGS}-D _DEBUG -ggdb3 -std=c++2a -O0 -Wall -Wextra\
    -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations\
    -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion\
    -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security\
    -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd\
    -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow\
    -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2\
    -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types\
    -Wsuggest-override -Wswitch-default -Wsync-nand -Wundef -Wunreachable-code\
    -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers\
    -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -Wswitch-enum\
    -fcheck-new -fsized-deallocation -fstack-check\
    -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -fPIE\
    -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr\
    -pie -Wlarger-than=8192 -Wstack-usage=8192") 

add_subdirectory(lib)
add_subdirectory(src)

install(TARGETS mbc_front mbc_mid mbc_back
        RUNTIME
        CONFIGURATIONS Release)

if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()