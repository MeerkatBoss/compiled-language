/**
 * @file benchmark.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief
 *
 * @version 0.1
 * @date 2023-04-27
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __TESTS_TEST_CASES_BENCHMARK_H
#define __TESTS_TEST_CASES_BENCHMARK_H

#include "test_utils/config.h"

/**
 * @brief Run benchmark on specified program
 *
 * @param[in] argc	    - Argument vector length
 * @param[in] argv	    - Argument vector
 * @param[in] config	- Test configuration
 *
 * @return Exit status
 */
int run_test_benchmark(int argc, const char* const* argv,
                       const TestConfig* config);

#endif /* benchmark.h */
