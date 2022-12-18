#ifndef BACK_FLAGS
#define BACK_FLAGS

#include "argparser.h"

struct arg_state
{
    const char* input_filename;
    const char* output_filename;
    bool no_stdlib;
    bool help_shown;
};

int back_set_input_file(const char* const* argv, void* params);
int back_set_output_file(const char* const* argv, void* params);
int back_set_no_stdlib(const char* const* argv, void* params);
int back_show_help(const char* const* argv, void* params);

const arg_tag BACK_TAGS[] = {
    {
        .short_tag = 'o',
        .long_tag = "output-file",
        .callback = back_set_output_file,
        .description = "Set output file. Default output file is \033[3m" "out.asm" "\033[23m."
    },
    {
        .short_tag = '\0',
        .long_tag = "no-stdlib",
        .callback = back_set_no_stdlib,
        .description = "Do not allow standard library functions."
    },
    {
        .short_tag = 'h',
        .long_tag = "help",
        .callback = back_show_help,
        .description = "Show help message."
    }
};

const arg_info BACK_ARG_INFO = {
    .help_message = "mbc_back [FLAG]... <FILE>",
    .name_handler = NULL,
    .plain_handler = back_set_input_file,
    .tags = BACK_TAGS,
    .tag_cnt = sizeof(BACK_TAGS)/sizeof(*BACK_TAGS)
};


#endif