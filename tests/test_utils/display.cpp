#include <stdio.h>
#include <math.h>

#include "./display.h"

static const size_t bar_length = 24;

void progress_bar(size_t done, size_t total, double last_ms)
{
    const double percentage = (double) done / (double) total;
    
    const size_t fill    = (size_t) round((double)bar_length * percentage);
    const size_t pending = (size_t) ceil((double)bar_length / (double)total);

    putchar('[');
    for (size_t i = 0; i < fill; ++i)
        putchar('#');
    for (size_t i = fill; i < fill + pending && i < bar_length; ++i)
        putchar('~');
    for (size_t i = fill + pending; i < bar_length; ++i)
        putchar(' ');
    putchar(']');

    printf(" [%.1lf%%]", percentage * 100);

    if (!isnan(last_ms))
        printf(" (%.2lfs remaining)", (double)(total - done) * last_ms / 1000);
    else
        printf(" %24s", "");

    putchar('\r');
    fflush(stdout);
}

