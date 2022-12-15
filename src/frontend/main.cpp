#include <stdio.h>

#include "logger.h"
#include "lexer.h"
#include "ast.h"

int main()
{
    add_default_file_logger();
    add_logger({
        .name = "Console logger",
        .stream = stdout,
        .logging_level = LOG_ERROR,
        .settings_mask = LGS_USE_ESCAPE | LGS_KEEP_OPEN
    });

    // dynamic_array(token) tokens = {};
    // array_ctor(&tokens);

    // lexer_tree tree = {};
    // lexer_tree_ctor(&tree);

    // #define LEXEME(name, str, ...) lexer_tree_add_word(&tree, str, TOK_##name);
    // #include "lexemes.h"
    // #undef LEXEME
    // lexer_tree_add_numbers(&tree);
    // lexer_tree_add_names(&tree);

    // parse_tokens(
    //     "aaa aa a main <%%>\n"
    //     "v====\n"
    //     "    int x = abc = 643 PTPTPTPT!!!\n"
    //     "    $300 <% abc is the same as 3 %>\n"
    //     "       print <% abc %> PTPTPTPT!!!\n"
    //     "   <<< 0.0PTPTPTPT!!!"
    //     "^====\n",
    // &tree, &tokens);

    // token_array_print(&tokens, stdout);

    // lexer_tree_dtor(&tree);
    // array_dtor(&tokens);

    abstract_syntax_tree tree = {};
    tree_read(&tree, stdin);
    tree_print(&tree, stdout);
    tree_dtor(&tree);
    putc('\n', stdout);

    return 0;
}