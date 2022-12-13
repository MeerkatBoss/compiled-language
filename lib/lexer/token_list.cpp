#include "token_list.h"
#define ARRAY_ELEMENT token

#include "dynamic_array_impl.h"

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
            #include "lexemes.h"
            LEXEME(NUM)
            LEXEME(NAME)
            LEXEME(EOF)
            LEXEME(ERROR)
            default: break;
        }
        
        #undef LEXEME
    }
}

token make_token(char *str, token_type type, size_t line_num, size_t char_num)
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
