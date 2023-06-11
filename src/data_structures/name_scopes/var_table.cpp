#include <string.h>

#include "var_table.h"
#define ARRAY_ELEMENT var_name

static inline void copy_element(ARRAY_ELEMENT* dest, const ARRAY_ELEMENT* src)
{
    *dest = *src;
}
static inline void delete_element(ARRAY_ELEMENT* element)
{
    *element = NULL;
}

#include "array/dynamic_array_impl.h"
#undef ARRAY_ELEMENT

void var_table_ctor(var_table *table, long offset, bool is_global)
{
    array_ctor(&table->vars);
    table->offset = offset;
    table->is_global = is_global;
}

void var_table_dtor(var_table *table)
{
    array_dtor(&table->vars);
    memset(table, 0, sizeof(*table));
}
