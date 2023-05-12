#ifndef TEXT_LINES_H
#define TEXT_LINES_H

/**
 * @file text_lines.h
 * @author MeerkatBoss
 * @brief Work with files line by line
 * @version 0.1
 * @date 2022-08-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stddef.h>

/**
 * @brief 
 * Single text line with additional information
 */
typedef struct
{
    size_t line_length;
    const char *line;
} Line;


/**
 * @brief 
 * Lines of a text file
 */
typedef struct
{
    const char *text;
    size_t text_len;
    Line *lines;
    size_t line_count;
} TextLines;

/**
 * @brief Create `TextLines` instance, reads specified file
 * 
 * @param[in] path - path to opened file
 * @return `TextLines` instance with stored file contents
 */
TextLines read_file(const char* path);

/**
 * @brief Free lines contained within `TextLines` instance
 * 
 * @param[in] txlines - TextLines instance
 */
void dispose_lines(TextLines* txlines);

/**
 * @brief Maps file contents to memory
 * 
 * @param[in] path Path to file
 * @param[out] file_size Pointer to file size
 * @param[in] map_extra Number of additional bytes to map
 * @return Number of mapped bytes
 */
size_t map_file(const char *path, char **text, size_t map_extra);

#endif
