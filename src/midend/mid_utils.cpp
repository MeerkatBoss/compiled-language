#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "logger.h"

#include "simplifier.h"

#include "mid_utils.h"

bool input_tree_from_file(const char *filename, abstract_syntax_tree *tree)
{
    LOG_ASSERT_ERROR(filename, return false, "Input file not specified.", NULL);
    LOG_ASSERT(tree, return false);

    FILE* input = fopen(filename, "r");
    LOG_ASSERT_ERROR(input, return false, "Failed to read file '%s': %s", filename, strerror(errno));
    tree_read(tree, input);
    fclose(input);

    LOG_ASSERT(tree->root, return false);
    return true;
}

bool try_simplify_tree(abstract_syntax_tree *tree)
{
    LOG_ASSERT_ERROR(simplify_tree(tree), return false, "Failed to simplify AST.", NULL);

    return true;
}

bool write_tree_to_file(const abstract_syntax_tree *tree, const char *filename)
{
    LOG_ASSERT(tree, return false);
    if (!filename) filename = MID_DEFAULT_OUTPUT;

    FILE* output = fopen(filename, "w+");
    LOG_ASSERT_ERROR(output, return false,
        "Failed to open file '%s': %s", filename, strerror(errno));
    tree_print(tree, output);
    putc('\n', output);

    fclose(output);
    return true;
}
