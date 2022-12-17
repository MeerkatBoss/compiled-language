#include <string.h>
#include <stdio.h>

#include "logger.h"

#include "table_stack.h"

#define ARRAY_ELEMENT var_table
static inline void copy_element(ARRAY_ELEMENT* dest, const ARRAY_ELEMENT* src) { memcpy(dest, src, sizeof(var_table)); }
static inline void delete_element(ARRAY_ELEMENT* element) { var_table_dtor(element); }
#include "dynamic_array_impl.h"
#undef ARRAY_ELEMENT


void table_stack_ctor(table_stack *tb_stack)
{
    array_ctor(&tb_stack->tables);
    tb_stack->var_cnt = 0;

    var_table global = {};
    var_table_ctor(&global, 0, true);

    array_push(&tb_stack->tables, global);
}

void table_stack_dtor(table_stack *tb_stack)
{
    array_dtor(&tb_stack->tables);
    memset(tb_stack, 0, sizeof(*tb_stack));
}

void table_stack_add_table(table_stack *tb_stack)
{
    LOG_ASSERT(tb_stack->tables.size > 0, return);

    var_table* last_table = array_back(&tb_stack->tables);

    size_t offset = 0;
    if (!last_table->is_global)
        offset = last_table->offset + last_table->vars.size;

    var_table added = {};
    var_table_ctor(&added, offset, false);

    array_push(&tb_stack->tables, added);
}

static size_t var_table_find(var_table* table, const char *var_name);

const char *table_stack_add_var(table_stack *tb_stack, const char *var_name, size_t *addr)
{
    static const char LABEL_FORMAT[] = ".%s.var_0x%zX";

    LOG_ASSERT(tb_stack->tables.size > 0, return);

    var_table* last_table = array_back(&tb_stack->tables);
    
    if (var_table_find(last_table, var_name) < last_table->vars.size) /* Variable already exists*/
        return NULL;

    int label_len = snprintf(NULL, 0, LABEL_FORMAT, var_name, tb_stack->var_cnt);

    char* label = (char*) calloc(label_len + 1, sizeof(*label));

    sprintf(label, LABEL_FORMAT, var_name, tb_stack->var_cnt);

    array_push(&last_table->vars, label);
    tb_stack->var_cnt++;

    *addr = last_table->offset + last_table->vars.size - 1;
    return label;
}

const char *table_stack_find_var(const table_stack *tb_stack, const char *var_name, bool *is_global)
{
    for (size_t i = tb_stack->tables.size; i > 0; i--)
    {
        var_table* cur_table = array_get_element(&tb_stack->tables, i - 1);
        size_t index = var_table_find(cur_table, var_name);
        if (index < cur_table->vars.size)
        {
            *is_global = cur_table->is_global;
            return *array_get_element(&cur_table->vars, index);
        }
    }

    return NULL;
}

static size_t var_table_find(var_table *table, const char *var_name)
{
    for (size_t i = 0; i < table->vars.size; i++)
    {
        size_t len = strlen(var_name);
        const char* cur_name = *array_get_element(&table->vars, i);
        if (strncmp(cur_name + 1, var_name, len) == 0 && cur_name[len + 1] == '.')
            return i;
    }
    return table->vars.size;
}
