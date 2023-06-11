/**
 * @file config.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief
 *
 * @version 0.1
 * @date 2023-04-27
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __TESTS_UTILS_CONFIG_H
#define __TESTS_UTILS_CONFIG_H

#include <stdio.h>

#include "meerkat_args/argparser.h"

enum TestCase
{
    TEST_NONE,
    TEST_BENCHMARK_FULL
};

struct TestConfig
{
    int had_error;
    TestCase test_case;
    const char* filename;
    int append_to_file;
};

/**
 * @brief Load common test configuration from command-line arguments
 *
 * @param[in]  argc	    - Length of argument vector
 * @param[in]  argv	    - Argument vector
 * @param[out] config	- Loaded configuration
 *
 * @return Number of arguments parsed upon success, -1 otherwise
 */
int configure_tests(int argc, const char* const* argv, TestConfig* config);

/**
 * @brief Select test case to be executed
 *
 * @param[in]    str    Parameter array
 * @param[inout] params TestConfig instance
 *
 * @return 1 upon success, -1 otherwise
 */
int test_select_test_case(const char* const* str, void* params);

/**
 * @brief Set output filename
 *
 * @param[in]    str    Parameter array
 * @param[inout] params TestConfig instance
 *
 * @return 1 upon success, -1 otherwise
 */
int test_set_output(const char* const* str, void* params);

/**
 * @brief Set output file mode to 'append'
 *
 * @param[in]    str    Parameter array
 * @param[inout] params TestConfig instance
 *
 * @return 0 upon success, -1 otherwise
 */
int test_set_append(const char* const* str, void* params);

/**
 * @brief Print help message and exit
 *
 * @param[in]    str    Parameter array
 * @param[inout] params TestConfig instance
 */
__attribute__((noreturn))
int test_get_help(const char* const* str, void* params);

static const arg_tag TEST_TAGS[] = {
    {
        .short_tag = 'o',
        .long_tag = "output-file",
        .callback = test_set_output,
        .description = 
            "Print output to specified file instead of stdout"
    },
    {
        .short_tag = '\0',
        .long_tag = "append",
        .callback = test_set_append,
        .description = 
            "If '-o' flag was specified, append to file instead of trunctating"
    },
    {
        .short_tag = 'h',
        .long_tag = "help",
        .callback = test_get_help,
        .description = 
            "Print help message and exit program"
    }
};

static const arg_info TEST_ARGS = {
    .help_message = 
        "hash_table_tests [-o <FILE> [-a]] <TEST CASE> [TEST OPTIONS]",
    .name_handler = NULL,
    .plain_handler = test_select_test_case,
    .tags = TEST_TAGS,
    .tag_cnt = sizeof(TEST_TAGS) / sizeof(*TEST_TAGS)
};

#endif /* config.h */
