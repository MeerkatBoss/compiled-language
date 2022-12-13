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

    lexer_tree_add_word(&tree, "aa aaa", TOK_ASSIGN);
    lexer_tree_add_word(&tree, "aa", TOK_VAR);
    lexer_tree_add_word(&tree, "bbb", TOK_NUM);
    lexer_tree_add_names(&tree);

    lexer_tree_build(&tree);

    parse_tokens("aa bb aa aaa bbb", &tree, &tokens);

    token_array_print(&tokens, stdout);

    lexer_tree_dtor(&tree);
    array_dtor(&tokens);

    return 0;
}