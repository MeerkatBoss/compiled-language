#include <string.h>

#include "logger.h"

#include "lexer.h"

struct file_pos
{
    size_t line;
    size_t column;
    size_t offset;
};


int parse_tokens(const char *str, const lexer_tree *tree, dynamic_array(token) * tokens)
{
    const lexer_node* cur_node = tree->root;

    file_pos current_pos = {0, 0, 0};
    file_pos token_pos = {0, 0, 0};
    file_pos complete_pos = {0, 0, 0};

    const lexer_node* complete = NULL;
    size_t str_length = strlen(str);

    while (current_pos.offset <= str_length)
    {
        unsigned char cur_c = str[current_pos.offset];
        file_pos lst_pos = current_pos;

        if (cur_c == '\n' || current_pos.offset == 0)
        {
            current_pos.line++;
            current_pos.column = 1;
        }
        else
            current_pos.column++;
        
        if (cur_node == tree->root) token_pos = current_pos;
        if (cur_node->type != TOK_ERROR)
        {
            complete = cur_node;
            complete_pos = current_pos;
        }

        if (cur_node->next[cur_c])
        {
            cur_node = cur_node->next[cur_c];
            current_pos.offset++;
            continue;
        }

        if (cur_node->type != TOK_ERROR)
        {
            size_t len = current_pos.offset - token_pos.offset;
            char* tok_str = strndup(str + token_pos.offset, len);

            array_push(tokens, make_token(
                                        tok_str,
                                        cur_node->type,
                                        token_pos.line,
                                        token_pos.column));
            cur_node = tree->root;

            current_pos = lst_pos;
            continue;
        }

        if (complete != NULL)
        {
            size_t len = complete_pos.offset - token_pos.offset;
            char* tok_str = strndup(str + token_pos.offset, len);

            array_push(tokens, make_token(
                                        tok_str,
                                        complete->type,
                                        token_pos.line,
                                        token_pos.column));

            cur_node = tree->root;
            complete = NULL;
            current_pos = complete_pos;
            complete_pos = {0, 0, 0};
            continue;
        }

        size_t len = current_pos.offset - token_pos.offset;

        LOG_ASSERT_ERROR(0, return -1,
            "Invalid token '%.*s' at line %zu column %zu",
            str + token_pos.offset, (int)len, token_pos.line, token_pos.column);
    }

    array_push(tokens, make_token(strdup(""), TOK_EOF, current_pos.line, current_pos.column));

    return 0;
}
