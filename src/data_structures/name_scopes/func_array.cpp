#include <string.h>

#include "util/logger/logger.h"
#include "data_structures/intermediate_repr/ir_dsl.h"
#include "func_array.h"

#define ARRAY_ELEMENT function

static inline void copy_element(ARRAY_ELEMENT* dest, const ARRAY_ELEMENT* src)
{
    memcpy(dest, src, sizeof(function));
}
static inline void delete_element(ARRAY_ELEMENT* element)
{
    memset(element, 0, sizeof(function));
}

#include "array/dynamic_array_impl.h"

#undef ARRAY_ELEMENT

void func_array_ctor(func_array *functions)
{
    array_ctor(functions);
}

int func_array_add_func(func_array *functions, const ast_node *func_node)
{
    LOG_ASSERT(func_node->type == NODE_NFUN, return -1);

    if (func_array_find_func(functions, func_node->value.name) != NULL)
        return -1;
    
    size_t args = 0;
    ast_node* arg = func_node->left;
    while (arg)
    {
        args++;
        arg = arg->right;
    }

    array_push(functions, { .node = func_node,
                            .ir_list_head = ir_node_new_empty(),
                            .name = func_node->value.name,
                            .arg_cnt = args});

    return 0;
}

const function *func_array_find_func(const func_array *functions, const char *name)
{
    for (size_t i = 0; i < functions->size; i++)
        if (strcmp(name, functions->data[i].name) == 0)
            return &functions->data[i];
    return NULL;
}
