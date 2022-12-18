#include <stdio.h>
#include <stdlib.h>

#include "logger.h"

#include "ast.h"
#include "compiler.h"

int main()
{
    add_default_file_logger();
    add_logger({
        .name = "Console logger",
        .stream = stderr,
        .logging_level = LOG_ERROR,
        .settings_mask = LGS_USE_ESCAPE | LGS_KEEP_OPEN
    });

    abstract_syntax_tree tree = {};

    FILE* input = fopen("ast.tree", "r");
    tree_read(&tree, input);
    fclose(input);

    FILE* output = fopen("result.asm", "w+");
    compiler_tree_to_asm(&tree, output, true);

    fclose(output);

    system("cat assets/__cmp_op.asm >> result.asm");
    system("cat assets/__logic_op.asm >> result.asm");
    system("cat assets/stdlib.asm >> result.asm");

    tree_dtor(&tree);

    return 0;
}