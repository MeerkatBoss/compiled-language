#include <stdio.h>
#include <string.h>

#include "argparser.h"
#include "logger.h"

static const arg_tag* find_tag(const char* str, const arg_info* infos);
static int compare_tag(const char* str, const arg_tag* tag);

bool parse_args(int argc, const char *const *argv, const arg_info *infos, void *params)
{
    LOG_ASSERT(argv != NULL, return false);
    LOG_ASSERT(infos != NULL, return false);

    if (infos->name_handler)
        LOG_ASSERT(infos->name_handler(argv, params) == 1, return false);
    argv++;

    while(*argv)
    {
        const arg_tag* tag = find_tag(*argv, infos);

        int parsed = 0;
        if (tag == NULL)
            parsed = infos->plain_handler(argv, params);
        else
            parsed = tag->callback(++argv, params);

        LOG_ASSERT_ERROR(
            parsed != -1, return -1,
            "Error parsing command-line option \'%s\'",
            argv[-( tag != NULL )]);
        
        argv += parsed;
    }

    return true;
}

void print_help(const arg_info* infos)
{
    puts(infos->help_message);
    putc('\n', stdout);

    const int FLAGS_WIDTH = 24; 
    for (size_t i = 0; i < infos->tag_cnt; i++)
    {
        int fill = FLAGS_WIDTH;

        LOG_ASSERT(infos->tags[i].short_tag != '\0'
                || infos->tags[i].long_tag != NULL, return);

        fputs("\033[1m", stdout);
        if (infos->tags[i].short_tag != '\0')
            fill -= printf("-%c", infos->tags[i].short_tag);

        if (infos->tags[i].long_tag != NULL)
        {
            if (infos->tags[i].short_tag != '\0')
            {
                fputs(", ", stdout);
                fill -= 2;
            }
            fill -= printf("--%s", infos->tags[i].long_tag);
        }
        fputs("\033[22m", stdout);
        printf("%*s%s\n", fill, "", infos->tags[i].description);
    }
}

static const arg_tag* find_tag(const char* str, const arg_info* infos)
{
    LOG_ASSERT(str[0] != '\0', return NULL);
    
    for (size_t i = 0; i < infos->tag_cnt; i++)
        if (compare_tag(str, &infos->tags[i]))
            return &infos->tags[i];

    return NULL;
}

static int compare_tag(const char* str, const arg_tag* tag)
{
    int n_dashes = 0;

    while(*str == '-')
    {
        str++;
        n_dashes++;
    }

    return
        (n_dashes == 1 && str[1] == '\0' && str[0] == tag->short_tag) ||
        (n_dashes == 2 && strcmp(str, tag->long_tag) == 0);
}

