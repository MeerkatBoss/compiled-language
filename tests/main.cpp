#include <stdio.h>

#include "test_utils/config.h"
#include "test_cases/benchmark.h"

int main(int argc, char** argv)
{
    TestConfig config = {};
    int parsed = configure_tests(argc, argv, &config);

    if (parsed < 0 || config.had_error)
    {
        fprintf(stderr, "Invalid arguments");
        return 1;
    }

    argc -= parsed - 1;
    argv += parsed - 1;

    switch (config.test_case)
    {
    case TEST_BENCHMARK_FULL:
        return run_test_benchmark(argc, argv, &config);
    case TEST_NONE:
    default:
        fprintf(stderr, "Invalid test case");
        return 1;
    }
}

