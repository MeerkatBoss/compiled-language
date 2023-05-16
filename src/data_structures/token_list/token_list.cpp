#include "util/logger/logger.h"

#include "token_list.h"
#define ARRAY_ELEMENT token

static inline void copy_element(ARRAY_ELEMENT* dest, const ARRAY_ELEMENT* src) { memcpy(dest, src, sizeof(token)); }
static inline void delete_element(ARRAY_ELEMENT* element) { free(element->str); memset(element, 0, sizeof(token)); }

#include "array/dynamic_array_impl.h"

#undef ARRAY_ELEMENT

void token_array_print(const dynamic_array(token) * tokens, FILE *stream)
{
    for (size_t i = 0; i < tokens->size; i++)
    {
        #define LEXEME(name, ...)                               \
            case TOK_##name:                                    \
                fprintf(stream, #name "('%s', %lg, %zu:%zu)\n", \
                    tokens->data[i].str,                        \
                    tokens->data[i].num,                        \
                    tokens->data[i].line_num,                   \
                    tokens->data[i].char_num);                  \
                break;
        
        switch (tokens->data[i].type)
        {
            #include "data_structures/types/lexemes.h"
            LEXEME(NUM)
            LEXEME(NAME)
            LEXEME(EOF)
            LEXEME(ERROR)
            default: break;
        }
        
        #undef LEXEME
    }
}

static inline void indent(size_t level, FILE* stream)
{
    for (size_t i = 0; i < level; i++) putc('\t', stream);
}

void token_array_generate_source(const dynamic_array(token) * tokens, FILE *stream)
{
    size_t indent_lvl = 0;
    bool require_indent = false;
    for (size_t i = 0; i < tokens->size; i++)
    {
        token* cur = array_get_element(tokens, i);
        token* nxt = (i < tokens->size - 1
                        ? array_get_element(tokens, i + 1)
                        : NULL);
        if (cur->type == TOK_BLOCK_START)
            indent_lvl++;
        else if (cur->type == TOK_BLOCK_END)
            indent_lvl--;

        if (require_indent)
        {
            indent(indent_lvl, stream);
            require_indent = false;
        }
        fputs(cur->str, stream);
        
        if (cur->type == TOK_BLOCK_START || cur->type == TOK_BLOCK_END || cur->type == TOK_STMT_END)
        {
            fputc('\n', stream);
            require_indent = true;
        }
        else if (nxt && nxt->type != TOK_COMMA && nxt->type != TOK_STMT_END)
            fputc(' ', stream);

    }
}

token token_ctor(char *str, token_type type, size_t line_num, size_t char_num)
{
    double num = 0;
    if (type == TOK_NUM)
    {
        char* endptr = NULL;
        num = strtod(str, &endptr);
        LOG_ASSERT_ERROR(*endptr == '\0', return {.type = TOK_ERROR},
                "Invalid number '%s'", str);
    }

    return {
        .type = type,
        .line_num = line_num,
        .char_num = char_num,
        .num = num,
        .str = str
    };
}

token make_token(token_type type)
{
    switch (type)
    {
    #define LEXEME(name, code)\
        case TOK_##name:            \
            return {                \
                .type = type,       \
                .line_num = 0,      \
                .char_num = 0,      \
                .num = 0,           \
                .str = strdup(code)};
    #include "data_structures/types/lexemes.h"
    #undef LEXEME
        case TOK_NUM:
            LOG_ASSERT(0 && "Cannot construct TOK_NUM without value.", return {});
            return {};
        case TOK_NAME:
            LOG_ASSERT(0 && "Cannot construct TOK_NAME without value.", return {});
            return {};
        default:
            LOG_ASSERT(0 && "Unknown enum value", return {});
            return {};
    }
    return {};
}

token make_token(double num)
{
    char* str = NULL;
    int size = snprintf(NULL, 0, "%.3lf", num);
    str = (char*) calloc(size + 1, sizeof(char));
    sprintf(str, "%.3f", num);
    return {
        .type = TOK_NUM,
        .line_num = 0,
        .char_num = 0,
        .num = num,
        .str = str
    };
}

token make_token(const char *str)
{
    return {
        .type = TOK_NAME,
        .line_num = 0,
        .char_num = 0,
        .num = 0,
        .str = strdup(str)
    };
}
