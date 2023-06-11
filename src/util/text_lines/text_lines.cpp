#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#include "text_lines.h"

/**
 * @brief Maps file lines to memory
 * 
 * @param[in] path Path to file
 * @param[out] file_size Pointer to file size
 * @param[in] map_extra Number of additional bytes to map
 * @return file contents
 */
size_t map_file(const char *path, char **text, size_t map_extra)
{
    //TODO: indicate errors by `memset`ting `text`
    assert(path != NULL);

    /* Open file descriptor */
    int fd = open(path, O_RDONLY);
    if (fd < 0)
        return 0;

    /* Get file size */
    struct stat file_stat = {};
    if (fstat(fd, &file_stat) < 0) /* Cannot read file, errno set by fstat */
    {
        int tmp = errno;
        close(fd);
        errno = tmp;
        return 0;
    }

    /* Call mmap */
    char *mapping = (char*) mmap(
        NULL, (size_t)file_stat.st_size + map_extra,
        PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    close(fd);
    if (mapping == MAP_FAILED) /* Mapping failed, errno set by mmap */
        return 0;

    *text = mapping;
    return (size_t)file_stat.st_size + map_extra;
}

static size_t split_text(char *text, size_t text_len)
{
    size_t line_count = 1;
    for (size_t i = 0; i < text_len - 1; i++)
        if (text[i] == '\n')
        {
            text[i] = '\0';
            line_count++;
        }
    text[text_len - 1] = '\0';
    return line_count;
}

TextLines read_file(const char *path)
{
    char *text = NULL;
    size_t text_len = map_file(path, &text, 1); /* Map extra byte for final '\0' */

    if (text == NULL)
        return {};

    size_t line_count = split_text(text, text_len);

    Line *lines = (Line*) calloc(line_count, sizeof(*lines));
    char *last_line = text; /* Pointer to the beginning of last line */
    size_t line_num = 0;
    for (size_t i = 0; i < text_len; i++)
        if (text[i] == '\0') /* Line ended */
        {
            lines[line_num] = {
                .line_length = (size_t)(&text[i] - last_line),
                .line = last_line
            };
            line_num++;                 /* Increment line number */
            last_line = &text[i + 1];   /* Next line begins on i+1st position */
        }

    return {
            .text = (const char*) text,
            .text_len = text_len,
            .lines = lines,
            .line_count = line_count
        };
}

void dispose_lines(TextLines *txlines)
{
    if (txlines == NULL || txlines->lines == NULL)
        return;

    munmap(const_cast<char *>(txlines->text), txlines->text_len);
    txlines->text = NULL;

    free((char **)txlines->lines);
    txlines->lines = NULL;
}
