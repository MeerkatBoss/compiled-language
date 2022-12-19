#include <stdio.h>

#include "logger.h"
#include "argparser.h"

#include "front_flags.h"
#include "front_utils.h"

#define STEP(action) LOG_ASSERT(action, return 1)

static int regular_flow(abstract_syntax_tree* tree, dynamic_array(token)* tokens, arg_state* state);
static int reverse_flow(abstract_syntax_tree* tree, dynamic_array(token)* tokens, arg_state* state);

int main(int argc, char** argv)
{
    add_default_file_logger();
    add_logger({
        .name = "Console logger",
        .stream = stderr,
        .logging_level = LOG_ERROR,
        .settings_mask = LGS_USE_ESCAPE | LGS_KEEP_OPEN
    });

    arg_state state = {};
    STEP(
        parse_args(argc, argv, &FRONT_ARG_INFO, &state)
    );

    if (state.help_shown) return 0;

    dynamic_array(token) tokens = {};
    array_ctor(&tokens);
    abstract_syntax_tree tree = {};

    int status = 0;
    if (state.reverse)
        status = reverse_flow(&tree, &tokens, &state);
    else
        status = regular_flow(&tree, &tokens, &state);

    tree_dtor(&tree);
    array_dtor(&tokens);

    return status;
}

int regular_flow(abstract_syntax_tree *tree, dynamic_array(token) * tokens, arg_state *state)
{
    STEP(
        get_lexemes_from_file(state->input_filename, tokens, state->show_tokens)
    );
    STEP(
        get_tree_from_lexemes(tokens, tree)
    );
    STEP(
        save_tree_to_file(tree, state->output_filename)
    );

    return 0;
}

int reverse_flow(abstract_syntax_tree *tree, dynamic_array(token) * tokens, arg_state *state)
{
    STEP(
        read_tree_from_file(state->input_filename, tree)
    );
    STEP(
        get_lexemes_from_tree(tree, tokens, state->show_tokens)
    );
    STEP(
        make_source_file(tokens, state->output_filename)
    );
    return 0;
}
