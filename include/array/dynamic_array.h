/**
 * @file dynamic_array.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * @brief Dynamic array function declarations
 * @version 0.1
 * @date 2022-12-03
 * 
 * @copyright Copyright (c) 2022
 * 
 * @note Before usage, provide definition for `ARRAY_ELEMENT` macro, as
 * well as for functions
 * `inline void copy_element(ARRAY_ELEMENT* dest, const ARRAY_ELEMENT* src);` and
 * `inline void delete_element(ARRAY_ELEMENT* element);`
 * 
 * @warning Definition of macro `ARRAY_ELEMENT` MUST NOT contain symbols, other than
 * decimal digits, latin alphabet letters and underscore. Definition of macro MUST NOT
 * begin with a decimal digit
 * 
 */

#ifndef ARRAY_ELEMENT
#error "Please provide array element type"
#endif

#include <stddef.h>

#include "_array_macros.h"

#define __DEF_HELPER(macro1, macro2) macro1(macro2)

__DEF_HELPER(DEFINE_DYNAMIC_ARRAY, ARRAY_ELEMENT);

__DEF_HELPER(DEFINE_ARRAY_CTOR,  ARRAY_ELEMENT);
__DEF_HELPER(DEFINE_ARRAY_DTOR,  ARRAY_ELEMENT);
__DEF_HELPER(DEFINE_ARRAY_PUSH,  ARRAY_ELEMENT);
__DEF_HELPER(DEFINE_ARRAY_POP,   ARRAY_ELEMENT);
__DEF_HELPER(DEFINE_ARRAY_COPY,  ARRAY_ELEMENT);
__DEF_HELPER(DEFINE_ARRAY_GET,   ARRAY_ELEMENT);
__DEF_HELPER(DEFINE_ARRAY_FRONT, ARRAY_ELEMENT);
__DEF_HELPER(DEFINE_ARRAY_BACK,  ARRAY_ELEMENT);

#undef __DEF_HELPER


