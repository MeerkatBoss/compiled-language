#include <stdio.h>

#include "logger.h"
#include "argparser.h"

#include "front_flags.h"
#include "front_utils.h"

#define STEP(action, cleanup) LOG_ASSERT(action, { cleanup; return 1;})

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
        parse_args(argc, argv, &FRONT_ARG_INFO, &state),
        {}
    );

    if (state.help_shown) return 0;

    dynamic_array(token) tokens = {};
    array_ctor(&tokens);
    abstract_syntax_tree tree = {};

    STEP(
        get_lexemes_from_file(state.input_filename, &tokens, state.show_tokens),
        { tree_dtor(&tree); array_dtor(&tokens); }
    );
    STEP(
        get_tree_from_lexemes(&tokens, &tree),
        { tree_dtor(&tree); array_dtor(&tokens); }
    );
    STEP(
        save_tree_to_file(&tree, state.output_filename),
        { tree_dtor(&tree); array_dtor(&tokens); }
    );

    tree_dtor(&tree);
    array_dtor(&tokens);

    return 0;
}