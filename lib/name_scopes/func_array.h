#ifndef FUNC_ARRAY_H
#define FUNC_ARRAY_H

#include "ast.h"

struct function
{
    const ast_node* node;
    const char* name;
    size_t arg_cnt;
};

#define ARRAY_ELEMENT function
#include "dynamic_array.h"
#undef ARRAY_ELEMENT

typedef dynamic_array(function) func_array;

void func_array_ctor(func_array* functions, bool use_stdlib = false);

inline void func_array_dtor(func_array* functions) { array_dtor(functions); }

int func_array_add_func(func_array* functions, const ast_node* func_node);

const function* func_array_find_func(const func_array* functions, const char* name);

#endif