#include <string.h>
#include <stdio.h>

#include "util/logger/logger.h"

#include "table_stack.h"

#define ARRAY_ELEMENT var_table
static inline void copy_element(ARRAY_ELEMENT* dest, const ARRAY_ELEMENT* src)
{
    memcpy(dest, src, sizeof(var_table));
}
static inline void delete_element(ARRAY_ELEMENT* element)
{
    var_table_dtor(element);
}
#include "array/dynamic_array_impl.h"
#undef ARRAY_ELEMENT


void table_stack_ctor(table_stack *tb_stack)
{
    array_ctor(&tb_stack->tables);
    tb_stack->var_cnt = 0;

    var_table global = {};
    // TODO: Extract
    var_table_ctor(&global, 0x401000, true);

    array_push(&tb_stack->tables, global);
}

void table_stack_dtor(table_stack *tb_stack)
{
    array_dtor(&tb_stack->tables);
    memset(tb_stack, 0, sizeof(*tb_stack));
}

bool table_stack_is_at_global_scope(table_stack* tb_stack)
{
    LOG_ASSERT(tb_stack->tables.size > 0, return false);

    var_table* last_table = array_back(&tb_stack->tables);
    return last_table->is_global;
}

long table_stack_get_next_offset(table_stack* tb_stack)
{
    LOG_ASSERT(tb_stack->tables.size > 0, return 0);
    
    var_table* last_table = array_back(&tb_stack->tables);
    if (last_table->offset <= 0)
        return 0;

    return last_table->offset + last_table->vars.size * 8;
}

void table_stack_add_table(table_stack *tb_stack, long offset)
{
    LOG_ASSERT(tb_stack->tables.size > 0, return);

    var_table* last_table = array_back(&tb_stack->tables);

    var_table added = {};
    var_table_ctor(&added, offset, false);

    array_push(&tb_stack->tables, added);
}

static size_t var_table_find(var_table* table, const char *name);

bool table_stack_add_var(table_stack *tb_stack, const char *name)
{
    LOG_ASSERT(tb_stack->tables.size > 0, return false);

    var_table* last_table = array_back(&tb_stack->tables);
    
    if (var_table_find(last_table, name) < last_table->vars.size)
        return false; /* Variable already exists*/

    array_push(&last_table->vars, name);

    return true;
}

bool table_stack_find_var(const table_stack* tb_stack, const char* name,
                          bool* is_global, long* addr)
{
    for (size_t i = tb_stack->tables.size; i > 0; i--)
    {
        var_table* cur_table = array_get_element(&tb_stack->tables, i - 1);
        size_t index = var_table_find(cur_table, name);
        if (index < cur_table->vars.size)
        {
            *is_global = cur_table->is_global;
            long offset = cur_table->offset + index*8;
            *addr = offset;
            return true;
        }
    }

    return false;
}

static size_t var_table_find(var_table *table, const char *name)
{
    for (size_t i = 0; i < table->vars.size; i++)
    {
        const char* cur_name = *array_get_element(&table->vars, i);
        if (strcmp(cur_name, name) == 0)
            return i;
    }
    return table->vars.size;
}
