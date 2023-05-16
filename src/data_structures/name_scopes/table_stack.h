#ifndef TABLE_STACK_ARRAY
#define TABLE_STACK_ARRAY

#include "var_table.h"

#define ARRAY_ELEMENT var_table

#include "array/dynamic_array.h"

#undef ARRAY_ELEMENT

/**
 * @brief Stack of name scopes
 */
struct table_stack
{
    dynamic_array(var_table) tables;
    size_t var_cnt;
};

/**
 * @brief Create new `table_stack` with single global scope
 *
 * @param[out] tb_stack     Constructed instance
 */
void table_stack_ctor(table_stack* tb_stack);

/**
 * @brief Destroy `table_stack` instance. Free associated resources.
 *
 * @param[inout] tb_stack   `table_stack` instance to be destroyed
 */
void table_stack_dtor(table_stack* tb_stack);

/**
 * @brief Retrieve the offset, corresponding to the space after
 * the last stored variable
 *
 * @param[inout] tb_stack   Scope stack 
 *
 * @return Retrieved offset
 */
long table_stack_get_next_offset(table_stack* tb_stack);

/**
 * @brief Add new local name scope to stack
 *
 * @param[inout] tb_stack   Scope stack 
 */
void table_stack_add_table(table_stack* tb_stack, long offset);

/**
 * @brief Add variable to the innermost name scope
 *
 * @param[inout] tb_stack   Scope stack
 * @param[in] name          Variable name
 *
 * @return `true` on successful addition, `false` otherwise
 *
 */
bool table_stack_add_var(table_stack* tb_stack, const char* name);

/**
 * @brief Remove innermost name scope from stack
 *
 * @param[inout] tb_stack   Scope stack
 */
inline void table_stack_pop_table(table_stack* tb_stack)
{
    array_pop(&tb_stack->tables);
}

/**
 * @brief Find variable with given name across scopes
 *
 * @param[in] table_stack   Scope stack
 * @param[in] name          Variable name
 * @param[out] is_global    `true` if variable has been defined in global scope,
 *                          `false` otherwise
 * @param[out] addr	        Variable offset or absolute address
 *
 * @return `true` if variable exists, `false` otherwise
 */
long table_stack_find_var(const table_stack* tb_stack, const char* name,
                                 bool* is_global, long* addr);

#endif
