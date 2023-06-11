/**
 * @file argparser.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * @brief Library with common command-line parsing functions.
 * @version 0.1
 * @date 2022-10-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef MEERKAT_ARGS_ARGPARSER_H
#define MEERKAT_ARGS_ARGPARSER_H

#include <stddef.h>

/**
 * @brief Callback used to parse argument parameters
 *
 * @param[in]    str    Parameter array
 * @param[inout] params Additional arguments
 *
 * @return -1 upon failure, number of parsed arguments upon success
 */
typedef int arg_callback(const char* const* str, void* params);

/**
 * @brief Command-line argument description
 */
struct arg_tag
{
    char            short_tag;
    const char*     long_tag;
    arg_callback*   callback;
    const char*     description;
};

struct arg_info
{
    const char*     help_message;
    arg_callback*   name_handler;
    arg_callback*   plain_handler;
    const arg_tag*  tags;
    size_t          tag_cnt;
};

/**
 * @brief Parse command-line arguments based on `arg_tag` array
 * and call corresponding callbacks
 * 
 * @param[in]    argc   Length of command-line argument vector
 * @param[in]    argv   Command-line arguments vector
 * @param[in]    info   Command-line argument handlers
 * @param[inout] params Additional parameter to pass to handlers
 *
 * @return Number of command-line options parsed, -1 upon invalid parameters
 */
int parse_args(int argc, const char* const* argv,
                const arg_info* info, void* params);

/**
 * @brief Print command-line options help based on array of
 * argument descriptions
 * 
 * @param[in] tag_count Length of `tags` array
 * @param[in] tags      Argument descriptions array
 */
void print_help(const arg_info* args);

#endif
