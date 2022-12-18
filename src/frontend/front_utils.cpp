#include <sys/mman.h>
#include <errno.h>

#include "text_lines.h"

#include "logger.h"
#include "lexer.h"
#include "parser.h"

#include "front_utils.h"

bool get_lexemes_from_file(const char *filename, dynamic_array(token) * tokens, bool print)
{
    LOG_ASSERT_ERROR(filename != NULL, return false, "No input file provided.", NULL);
    LOG_ASSERT(tokens != NULL, return false);

    char* contents = NULL;

    size_t size = map_file(filename, &contents, 1);

    LOG_ASSERT(contents != NULL, return false);

    lexer_tree lex_tree = {};
    lexer_tree_ctor(&lex_tree);

#define LEXEME(name, str, ...) lexer_tree_add_word(&lex_tree, str, TOK_##name);
    #include "lexemes.h"
#undef LEXEME
    lexer_tree_add_numbers(&lex_tree);
    lexer_tree_add_names(&lex_tree);

    LOG_ASSERT(parse_tokens(contents, &lex_tree, tokens),
        { lexer_tree_dtor(&lex_tree); return false; });
    
    lexer_tree_dtor(&lex_tree);
    munmap(contents, size);

    if (print) token_array_print(tokens, stdout);

    return true;
}

bool get_tree_from_lexemes(const dynamic_array(token) * tokens, abstract_syntax_tree *tree)
{
    LOG_ASSERT(tokens, return false);
    LOG_ASSERT(tree, return false);
    return parser_build_tree(tokens, tree) == 0;
}

bool save_tree_to_file(const abstract_syntax_tree *tree, const char *filename)
{
    LOG_ASSERT(tree, return false);
    if (!filename) filename = FRONT_DEFAULT_OUTPUT;

    FILE* output = fopen(filename, "w+");
    LOG_ASSERT_ERROR(output, return false,
        "Failed to open file '%s': %s", filename, strerror(errno));
    tree_print(tree, output);
    putc('\n', output);

    fclose(output);
    return true;
}
