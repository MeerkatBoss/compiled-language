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
    file_pos pos = {0, 0, 0};
    file_pos tok_start = {0, 0, 0};
    size_t str_length = strlen(str);

    for (; pos.offset <= str_length; pos.offset++)
    {
        char cur_c = str[pos.offset];

        // if (cur_c == '\n' || pos.offset == 0)
        // {
        //     pos.line++;
        //     pos.column = 1;
        // }
        // else
        //     pos.column++;
        
        if (cur_node == tree->root) tok_start = pos;

        if (cur_node->next[cur_c])
        {
            cur_node = cur_node->next[cur_c];
            continue;
        }

        if (cur_node->type != TOK_ERROR)
        {
            size_t len = pos.offset - tok_start.offset;
            array_push(tokens, {
                            .type = cur_node->type,
                            .line_num = tok_start.line,
                            .char_num = tok_start.column,
                            .str = strndup(str + tok_start.offset, len)
                        });
            cur_node = tree->root;

            if (cur_c != '\0') pos.offset--;

            continue;
        }

        if (cur_node->prev_complete != NULL)
        {
            size_t len = pos.offset - tok_start.offset - cur_node->excess;

            array_push(tokens, {
                            .type = cur_node->prev_complete->type,
                            .line_num = tok_start.line,
                            .char_num = tok_start.column,
                            .str = strndup(str + tok_start.offset, len)
                        });
            cur_node = tree->root;
            pos.offset -= cur_node->excess + 1;

            continue;
        }

        size_t len = pos.offset - tok_start.offset - cur_node->excess;

        LOG_ASSERT_ERROR(0, return -1,
            "Invalid token '%.*s' at char %zu",
            str + pos.offset - len, (int)len, tok_start.offset);
    }

    array_push(tokens, {
                    .type = TOK_EOF,
                    .line_num = pos.line,
                    .char_num = pos.column,
                    .str = strdup("")
                });

    return 0;
}
