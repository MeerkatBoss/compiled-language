#include <stdio.h>

#include "util/logger/logger.h"

#include "mid_flags.h"
#include "mid_utils.h"

#define STEP(action, cleanup) LOG_ASSERT(action, { cleanup; return 1; })

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
        parse_args(argc, argv, &MID_ARG_INFO, &state), {}
    );

    if (state.help_shown) return 0;

    abstract_syntax_tree tree = {};
    STEP(
        input_tree_from_file(state.input_filename, &tree), {}
    );
    STEP(
        try_simplify_tree(&tree), tree_dtor(&tree)
    );
    STEP(
        write_tree_to_file(&tree, state.output_filename), tree_dtor(&tree)
    );

    tree_dtor(&tree);
    return 0;
}