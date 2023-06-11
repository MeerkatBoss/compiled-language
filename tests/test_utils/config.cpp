#include <stdlib.h>
#include <strings.h>
#include <stdio.h>

#include "config.h"

int configure_tests(int argc, const char* const* argv, TestConfig* config)
{
    config->had_error = 0;
    config->test_case = TEST_NONE;
    config->filename = NULL;
    config->append_to_file = 0;

    int parsed = parse_args(argc, argv, &TEST_ARGS, config);

    if (config->had_error)
        return -1;
    
    return parsed;
}

int test_select_test_case(const char* const* str, void* params)
{
    TestConfig* config = (TestConfig*) params;
    if (config->test_case != TEST_NONE)
        return -1;
    
    const char* test_name = *str;

    if (strcasecmp(test_name, "benchmark") == 0)
    {
        config->test_case = TEST_BENCHMARK_FULL;
        return 1;
    }

    fprintf(stderr, "Error: unknown test case '%s'\n", test_name);
    config->had_error = 1;
    return -1;
}

int test_set_output(const char* const* str, void* params)
{
    TestConfig* config = (TestConfig*) params;
    if (config->test_case != TEST_NONE)
        return -1;

    if (config->filename)
    {
        fprintf(stderr, "Error: output file specified more than once\n");
        config->had_error = 1;
        return -1;
    }

    config->filename = *str;
    return 1;
}

int test_set_append(const char* const* str, void* params)
{
    TestConfig* config = (TestConfig*) params;
    if (config->test_case != TEST_NONE)
        return -1;

    if (config->append_to_file)
    {
        fprintf(stderr, "Error: '--append' flag specified more than once\n");
        config->had_error = 1;
        return -1;
    }

    config->append_to_file = 1;
    return 1;
}

__attribute__((noreturn))
int test_get_help(const char* const* str, void* params)
{
    print_help(&TEST_ARGS);
    exit(0);
}

