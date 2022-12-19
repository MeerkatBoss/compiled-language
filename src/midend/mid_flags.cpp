#include "logger.h"

#include "mid_flags.h"

int mid_set_input_file(const char *const *argv, void *params)
{
    arg_state* state = (arg_state*)params;

    LOG_ASSERT_ERROR(state->input_filename == NULL, return -1,
            "Attempted to redefine input file '%s' to '%s'", state->input_filename);

    state->input_filename = *argv;

    return 1;
}

int mid_set_output_file(const char *const *argv, void *params)
{
    arg_state* state = (arg_state*)params;

    LOG_ASSERT_ERROR(state->output_filename == NULL, return -1,
            "Attempted to redefine output file '%s' to '%s'", state->output_filename);

    state->output_filename = *argv;

    return 1;
}

int mid_show_help(const char *const *, void *params)
{
    arg_state* state = (arg_state*)params;
    print_help(&MID_ARG_INFO);
    state->help_shown = true;
    return 0;
}
