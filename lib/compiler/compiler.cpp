#include "table_stack.h"

#include "func_array.h"
#include "compiler.h"

struct compilation_state
{
    const char* func_name;
    size_t block_depth;
    bool has_return;
};

int compiler_tree_to_asm(const abstract_syntax_tree *tree, FILE *output)
{
    return 0;
}
