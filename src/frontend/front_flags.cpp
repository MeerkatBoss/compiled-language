#include "logger.h"

#include "front_flags.h"

int set_input_file(const char *const *argv, void *params)
{
    arg_state* state = (arg_state*)params;

    LOG_ASSERT_ERROR(state->input_filename == NULL, return -1,
            "Attempted to redefine input file '%s' to '%s'", state->input_filename);

    state->input_filename = *argv;

    return 1;
}

int set_output_file(const char *const *argv, void *params)
{
    arg_state* state = (arg_state*)params;

    LOG_ASSERT_ERROR(state->output_filename == NULL, return -1,
            "Attempted to redefine output file '%s' to '%s'", state->output_filename);

    state->output_filename = *argv;

    return 1;
}

int set_reverse(const char *const *, void *params)
{
    arg_state* state = (arg_state*)params;
    state->reverse = true;
    return 0;
}

int set_show_tokens(const char *const *, void *params)
{
    arg_state* state = (arg_state*)params;
    state->show_tokens = true;
    return 0;
}

int show_help(const char *const *, void *params)
{
    arg_state* state = (arg_state*)params;
    print_help(&ARG_INFO);
    state->help_shown = true;
    return 0;
}
