#ifndef VAR_TABLE_H
#define VAR_TABLE_H

#include <stdlib.h>

typedef const char* var_name;

#define ARRAY_ELEMENT var_name

#include "array/dynamic_array.h"
#undef ARRAY_ELEMENT

struct var_table
{
    dynamic_array(var_name) vars;
    long offset;
    bool is_global;
};

/**
 * @brief Create new name table
 * @param[out] table Constructed `var_table`
 * @param[in] offset Address offset for first variable in scope
 * @param[in] is_global `true` if name scope should be global, `false` otherwise
 */
void var_table_ctor(var_table* table, long offset, bool is_global);

/**
 * @brief Destroy name table
 * @param[inout] table `var_table` instance to be destroyed
 */
void var_table_dtor(var_table* table);

#endif
