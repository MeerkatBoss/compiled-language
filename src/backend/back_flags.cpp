#include "logger.h"

#include "back_flags.h"

int back_set_input_file(const char *const *argv, void *params)
{
    arg_state* state = (arg_state*)params;

    LOG_ASSERT_ERROR(state->input_filename == NULL, return -1,
            "Attempted to redefine input file '%s' to '%s'", state->input_filename);

    state->input_filename = *argv;

    return 1;
}

int back_set_output_file(const char *const *argv, void *params)
{
    arg_state* state = (arg_state*)params;

    LOG_ASSERT_ERROR(state->output_filename == NULL, return -1,
            "Attempted to redefine output file '%s' to '%s'", state->output_filename);

    state->output_filename = *argv;

    return 1;
}

int back_set_no_stdlib(const char *const *, void *params)
{
    arg_state* state = (arg_state*)params;
    state->no_stdlib = true;
    return 0;
}

int back_show_help(const char *const *, void *params)
{
    arg_state* state = (arg_state*)params;
    print_help(&BACK_ARG_INFO);
    state->help_shown = true;
    return 0;
}
