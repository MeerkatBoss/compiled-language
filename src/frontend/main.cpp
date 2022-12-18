#include <stdio.h>

#include "logger.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"

int main()
{
    add_default_file_logger();
    add_logger({
        .name = "Console logger",
        .stream = stderr,
        .logging_level = LOG_ERROR,
        .settings_mask = LGS_USE_ESCAPE | LGS_KEEP_OPEN
    });

    dynamic_array(token) tokens = {};
    array_ctor(&tokens);

    lexer_tree lex_tree = {};
    lexer_tree_ctor(&lex_tree);

    #define LEXEME(name, str, ...) lexer_tree_add_word(&lex_tree, str, TOK_##name);
    #include "lexemes.h"
    #undef LEXEME
    lexer_tree_add_numbers(&lex_tree);
    lexer_tree_add_names(&lex_tree);

    parse_tokens(
        "fu n main(0\n"
        "[\n"
        "   set_pixel(-0.02, 0.05, 1 0'\n"
        "   set_pixel( 0.02, 0.05, 1 0'\n"
        "   set_pixel(-0.04, 0.0, 1 0'\n"
        "   set_pixel(-0.02, 0.0, 1 0'\n"
        "   set_pixel( 0.00, 0.0, 1 0'\n"
        "   set_pixel( 0.02, 0.0, 1 0'\n"
        "   set_pixel( 0.04, 0.0, 1 0'\n"
        "   set_pixel(-0.02, -0.05, 1 0'\n"
        "   set_pixel( 0.00, -0.05, 1 0'\n"
        "   set_pixel( 0.02, -0.05, 1 0'\n"
        "   set_pixel(0.0, -0.1, 1 0'\n"
        "   flush(0'"
        "   riturn 0.0'"
        "}\n",
    &lex_tree, &tokens);

    // token_array_print(&tokens, stdout);

    abstract_syntax_tree tree = {};
    parser_build_tree(&tokens, &tree);
    FILE* output = fopen("ast.tree", "w+");
    tree_print(&tree, output);
    putc('\n', output);

    tree_dtor(&tree);
    lexer_tree_dtor(&lex_tree);
    array_dtor(&tokens);
    fclose(output);

    return 0;
}