#include <errno.h>
#include <string.h>

#include "util/logger/logger.h"

#include "compiler/compiler.h"

#include "back_utils.h"

bool get_tree_from_file(const char *filename, abstract_syntax_tree *tree)
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

bool compile_tree_to_file(const abstract_syntax_tree *tree, const char *filename, bool use_stdlib)
{
    LOG_ASSERT(tree, return false);
    if (!filename) filename = BACK_DEFAULT_OUTPUT;
    FILE* output = fopen(filename, "w+");
    LOG_ASSERT_ERROR(output, return false,
        "Failed to open file '%s': %s", filename, strerror(errno));

    bool success = compiler_tree_to_asm(tree, output, use_stdlib);
    fclose(output);

    LOG_ASSERT(success, return false);

    return true;
}
