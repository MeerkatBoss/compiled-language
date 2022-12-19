#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "logger.h"

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
