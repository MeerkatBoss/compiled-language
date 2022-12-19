#ifndef MID_FLAGS
#define MID_FLAGS

#include "argparser.h"

struct arg_state
{
    const char* input_filename;
    const char* output_filename;
    bool help_shown;
};

int mid_set_input_file(const char* const* argv, void* params);
int mid_set_output_file(const char* const* argv, void* params);
int mid_show_help(const char* const* argv, void* params);

const arg_tag MID_TAGS[] = {
    {
        .short_tag = 'o',
        .long_tag = "output-file",
        .callback = mid_set_output_file,
        .description = "Set output file. Default output file is \033[3m" "out.asm" "\033[23m."
    },
    {
        .short_tag = 'h',
        .long_tag = "help",
        .callback = mid_show_help,
        .description = "Show help message."
    }
};

const arg_info MID_ARG_INFO = {
    .help_message = "mbc_back [FLAG]... <FILE>",
    .name_handler = NULL,
    .plain_handler = mid_set_input_file,
    .tags = MID_TAGS,
    .tag_cnt = sizeof(MID_TAGS)/sizeof(*MID_TAGS)
};

#endif