#include <stdio.h>

#include "logger.h"
#include "lexer.h"

int main()
{
    add_default_file_logger();
    add_logger({
        .name = "Console logger",
        .stream = stdout,
        .logging_level = LOG_ERROR,
        .settings_mask = LGS_USE_ESCAPE | LGS_KEEP_OPEN
    });

    dynamic_array(token) tokens = {};
    array_ctor(&tokens);

    lexer_tree tree = {};
    lexer_tree_ctor(&tree);

    lexer_tree_add_word(&tree, "= 64", TOK_ASSIGN);
    lexer_tree_add_word(&tree, "int x = ", TOK_VAR);
    lexer_tree_add_numbers(&tree);
    lexer_tree_add_names(&tree);

    parse_tokens("int x = y = 6451", &tree, &tokens);

    token_array_print(&tokens, stdout);

    lexer_tree_dtor(&tree);
    array_dtor(&tokens);

    return 0;
}