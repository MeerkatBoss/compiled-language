#include "token_list.h"
#define ARRAY_ELEMENT token

#include "dynamic_array_impl.h"

#undef ARRAY_ELEMENT

void token_array_print(const dynamic_array(token) * tokens, FILE *stream)
{
    for (size_t i = 0; i < tokens->size; i++)
    {
        #define LEXEME(name, ...) \
            case TOK_##name: fprintf(stream, #name "(%s)\n", tokens->data[i].str); break;
        
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
