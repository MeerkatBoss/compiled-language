#ifndef FRONT_FLAGS
#define FRONT_FLAGS

#include "argparser.h"

struct arg_state
{
    const char* input_filename;
    const char* output_filename;
    bool reverse;
    bool show_tokens;
    bool help_shown;
};

int set_input_file(const char* const* argv, void* params);
int set_output_file(const char* const* argv, void* params);
int set_reverse(const char* const* argv, void* params);
int set_show_tokens(const char* const* argv, void* params);
int show_help(const char* const* argv, void* params);

const arg_tag TAGS[] = {
    {
        .short_tag = 'o',
        .long_tag = "output-file",
        .callback = set_output_file,
        .description = "Set output file. Default output file is \033[3m" "out.ast" "\033[23m."
    },
    {
        .short_tag = 'r',
        .long_tag = "reverse",
        .callback = set_reverse,
        .description = "Convert AST file back to code"
    },
    {
        .short_tag = 't',
        .long_tag = "list-tokens",
        .callback = set_show_tokens,
        .description = "Print lexeme list before compilation"
    },
    {
        .short_tag = 'h',
        .long_tag = "help",
        .callback = show_help,
        .description = "Show help message"
    }
};

const arg_info ARG_INFO = {
    .help_message = "mbc_front [FLAG]... <FILE>",
    .name_handler = NULL,
    .plain_handler = set_input_file,
    .tags = TAGS,
    .tag_cnt = sizeof(TAGS)/sizeof(*TAGS)
};


#endif