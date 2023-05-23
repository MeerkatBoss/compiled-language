#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <unistd.h>

#include "meerkat_assert/asserts.h"

#include "test_utils/math.h"
#include "test_utils/display.h"

#include "benchmark.h"

struct ExecTime
{
    double user_ms;
    double sys_ms;
};


static ssize_t get_repeat(const char* str);

static int fill_data(int argc, char** argv,
                     double* sys_time, double* user_time, size_t data_size);

static int get_execution_time(int argc, char** argv, ExecTime* exec_time);

static void run_child(int argc, char** argv);


int run_test_benchmark(int argc, const char* const* argv,
                       const TestConfig* config)
{
    size_t repeat_count = 10;
    FILE *output = NULL;
    double *sys_time = NULL, *user_time = NULL;

    SAFE_BLOCK_START
    {
        if (config->filename)
        {
            ASSERT_MESSAGE(
                output = fopen(config->filename,
                                config->append_to_file ? "a" : "w"),
                action_result != NULL,
                "Failed to open output file");
        }
        else output = stdout;

        ASSERT_GREATER_MESSAGE(
            argc, 1, "No program to benchmark");

        ssize_t parsed = get_repeat(argv[1]);
        if (parsed >= 0)
        {
            argc--;
            argv++;
            ASSERT_GREATER_MESSAGE(
                argc, 1, "No program to benchmark");
            ASSERT_POSITIVE_MESSAGE(
                parsed, "Benchmark repeat count must be positive");

            repeat_count = (size_t) parsed;
        }

        ASSERT_MESSAGE(
            sys_time = (double*) calloc(repeat_count, sizeof(*sys_time)),
            action_result != NULL,
            "Failed to allocate memory");
        ASSERT_MESSAGE(
            user_time = (double*) calloc(repeat_count, sizeof(*user_time)),
            action_result != NULL,
            "Failed to allocate memory");

        -- argc;
        ++ argv;
        ASSERT_ZERO_MESSAGE(
            fill_data(argc, const_cast<char**>(argv),
                        sys_time, user_time, repeat_count),
            "Failed to run benchmark");
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        fprintf(stderr, "Error: %s\n", assertion_info.message);
        return 1;
    }
    SAFE_BLOCK_END

    double mean_sys   = get_mean(sys_time,  repeat_count);
    double mean_user  = get_mean(user_time, repeat_count);
    double mean_total = mean_sys + mean_user;

    double err_sys   = get_stddev(sys_time,  mean_sys,  repeat_count);
    double err_user  = get_stddev(user_time, mean_user, repeat_count);
    double err_total = err_sys + err_user;

    const double sys_exp   = get_round_exponent(err_sys);
    const double user_exp  = get_round_exponent(err_user);
    const double total_exp = get_round_exponent(err_total);

    mean_sys   = round_with_exponent(mean_sys,   sys_exp);
    mean_user  = round_with_exponent(mean_user,  user_exp);
    mean_total = round_with_exponent(mean_total, total_exp);

    err_sys   = round_with_exponent(err_sys,   sys_exp);
    err_user  = round_with_exponent(err_user,  user_exp);
    err_total = round_with_exponent(err_total, total_exp);

    putchar('\n');
    fprintf(output, "Total: %lgms (~%lgms)\n"
                    "User:  %lgms (~%lgms)\n"
                    "Sys:   %lgms (~%lgms)\n",
                    mean_total, err_total,
                    mean_user,  err_user,
                    mean_sys,   err_sys);

    free(sys_time);
    free(user_time);

    return 0;
}

static ssize_t get_repeat(const char* str)
{
    char* end = NULL;

    long parsed = strtol(str, &end, 10);

    if (parsed >= 0 && *end == '\0')
        return (size_t) parsed;

    return -1;
}

static int fill_data(int argc, char** argv,
                     double* sys_time, double* user_time, size_t data_size)
{
    double last_ms = NAN;
    const size_t repeat = 5;

    for (size_t i = 0; i < data_size; ++i)
    {
        double sum_sys = 0, sum_user = 0;
        for (size_t j = 0; j < repeat; j++)
        {
            progress_bar(i*repeat + j, data_size * repeat, last_ms);

            ExecTime exec_time = {};
            if (get_execution_time(argc, argv, &exec_time) < 0)
                return -1;
            
            sum_sys  += exec_time.sys_ms;
            sum_user += exec_time.user_ms;

            last_ms = exec_time.sys_ms + exec_time.user_ms;
        }

        sys_time[i]  = sum_sys  / repeat;
        user_time[i] = sum_user / repeat;
    }

    progress_bar(data_size * repeat, data_size*repeat, NAN);

    return 0;
}

__always_inline
static double get_utime(rusage* usage)
{
    return (double) usage->ru_utime.tv_sec * 1000.0
         + (double) usage->ru_utime.tv_usec / 1000.0;
}

__always_inline
static double get_stime(rusage* usage)
{
    return (double) usage->ru_stime.tv_sec * 1000.0
         + (double) usage->ru_stime.tv_usec / 1000.0;
}

static int get_execution_time([[maybe_unused]]int argc, char** argv,
                                              ExecTime* exec_time)
{
    pid_t child = fork();

    if (child < 0)
    {
        perror("Failed to spawn proccess");
        return -1;
    }

    if (child == 0)
        run_child(argc, argv);

    rusage usage = {};
    double sum_sys = 0, sum_user = 0;

    getrusage(RUSAGE_CHILDREN, &usage);
    sum_user = get_utime(&usage);
    sum_sys  = get_stime(&usage);
    
    int status = 0;
    if (waitpid(child, &status, 0) < 0)
    {
        perror("Failed to wait for child");
        return -1;
    }

    getrusage(RUSAGE_CHILDREN, &usage);

    exec_time->user_ms = get_utime(&usage) - sum_user;
    exec_time->sys_ms  = get_stime(&usage) - sum_sys;

    return 0;
}

__attribute__((noreturn))
static void run_child(int argc, char** argv)
{
    int dev_null = open("/dev/null", O_WRONLY);

    if (dev_null < 0)   // On Windows
        dev_null = open("nul", O_WRONLY);

    // Redirect output to /dev/null or nul
    dup2(dev_null, STDOUT_FILENO);

    if (execvp(argv[0], argv) < 0)
    {
        perror("Failed to start program");
        exit(1);
    }

    /* Unreachable */
    exit(0);
}
