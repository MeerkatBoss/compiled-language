/**
 * @file math.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief
 *
 * @version 0.1
 * @date 2023-04-28
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __TESTS_TEST_UTILS_MATH_H
#define __TESTS_TEST_UTILS_MATH_H

#include <stddef.h>

/**
 * @brief Calculate the average of values
 *
 * @param[in] data      - Values to calculate average of
 * @param[in] data_size	- Length of `data`
 *
 * @return Average value in `data`
 */
double get_mean(const double* data, size_t data_size);

/**
 * @brief Calculate the standard deviation of values
 *
 * @param[in] data      - Values to calculate standard deviation of
 * @param[in] mean	    - Average value in `data`
 * @param[in] data_size	- Length of `data`
 *
 * @return Standard deviation in `data`
 */
double get_stddev(const double* data, double mean, size_t data_size);

/**
 * @brief Get exponent for rounding to most significant digit
 *
 * @param[in] value	- Value to be rounded
 *
 * @return Value of exponent (base 10)
 */
double get_round_exponent(double value);

/**
 * @brief Round value using exponent
 *
 * @param[in] value	    - Value to be rounded
 * @param[in] exponent	- Rounding exponent
 *
 * @return Rounded value
 */
double round_with_exponent(double value, double exponent);

#endif /* math.h */
