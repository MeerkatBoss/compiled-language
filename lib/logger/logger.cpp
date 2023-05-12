#include <math.h>
#include <stdarg.h>
#include <time.h>

#include "logger.h"
#include "text_styles.h"

#define MSG_TRACE_TEXT  "~trace~"
#define MSG_INFO_TEXT   "Info"
#define MSG_WARNING_TEXT "WARN"
#define MSG_ERROR_TEXT  "ERROR"
#define MSG_FATAL_TEXT  "!!FATAL!!"

static size_t loggers_count_ = 0;
static logger* loggers_[MAX_LOGGERS_COUNT];
static int paused = 0;

static int trash_ = atexit(log_stop);

void add_logger(logger added)
{
    LOG_ASSERT(
        loggers_count_ < MAX_LOGGERS_COUNT,
        {return;});

    logger* log_ptr = (logger*)calloc(1, sizeof(*log_ptr));

    LOG_ASSERT(
        log_ptr != NULL,
        {return;});

    *log_ptr = added;
    add_custom_logger(log_ptr);
    // loggers_[loggers_count_++] = log_ptr;

    // setbuf(added.stream, NULL);

    // if(added.settings_mask & LGS_USE_HTML)
    // {
    //     fputs("<!DOCTYPE html>",added.stream);
    //     fprintf(added.stream, "<head><title>%s</title></head>", added.name);
    //     fputs("<body><pre>", added.stream);
    // }
}

void add_custom_logger(logger* added)
{
    LOG_ASSERT(
        loggers_count_ < MAX_LOGGERS_COUNT,
        {return;});

    loggers_[loggers_count_++] = added;   
    setbuf(added->stream, NULL);

    if(added->settings_mask & LGS_USE_HTML)
    {
        // fputs("<!DOCTYPE html>\n",added->stream);
        // fprintf(added->stream, "<head><title>%s</title></head>\n", added->name);
        fputs("<body style=\"background: midnightblue; color: lavender\"><pre>\n", added->stream);
    }
    log_message(MSG_INFO, "Started logging at \'%s\'", added->name);
}

void add_default_file_logger(void)
{
    add_logger({
        .name           = "Default file logger",
        .stream         = fopen("default.log", "a"),
        .logging_level  = LOG_ALL,
        .settings_mask  = 0
    });
}

void add_default_console_logger(void)
{
    add_logger({
        .name           = "Default console logger",
        .stream         = stdout,
        .logging_level  = LOG_INFO,
        .settings_mask  = LGS_KEEP_OPEN | LGS_USE_ESCAPE
    });
}

void log_message(message_level level, const char* format, ...)/* TODO: very messy code, fix*/
{
    #define COLORED_CASE(var, type, setting) case MSG_##type:\
            var = TEXT_##setting##_##type (MSG_##type##_TEXT); break;
    #define ALL_CASES(lvl, var, setting) switch(lvl)\
    {\
        COLORED_CASE(var, TRACE,   setting)\
        COLORED_CASE(var, INFO,    setting)\
        COLORED_CASE(var, WARNING, setting)\
        COLORED_CASE(var, ERROR,   setting)\
        COLORED_CASE(var, FATAL,   setting)\
        default:\
            msg_type = "??UNKNOWN??";\
            break;\
    }

    const int MAX_DATE_SIZE = 32;
    char time_str[MAX_DATE_SIZE] = "";
    time_t cur_time = time(NULL);
    struct tm time_struct = {};
    localtime_r(&cur_time, &time_struct);
    strftime(time_str, MAX_DATE_SIZE, "%F %T%z", &time_struct);

    va_list vlist = {};
    va_start(vlist, format);

    for (size_t i = 0; i < loggers_count_; i++)
    {
        logger* current_logger = loggers_[i];
        if ((paused && !(current_logger->settings_mask & LGS_LOG_ALWAYS))
                || (int)current_logger->logging_level > (int)level)
            continue;
        
        if (current_logger->settings_mask & LGS_USE_ESCAPE)
        {
            const char* msg_type = "";
            ALL_CASES(level, msg_type, ESCAPED)
            fprintf(current_logger->stream,
                    TEXT_ESCAPED_NOTE("%s") "\t[%s]:\t",
                    time_str, msg_type);
        }
        else if (current_logger->settings_mask & LGS_USE_HTML)
        {
            const char* msg_type = "";
            ALL_CASES(level, msg_type, HTML)
            fprintf(current_logger->stream,
                    TEXT_HTML_NOTE("%s") "\t[%s]:\t",
                    time_str, msg_type);
        }
        else
        {
            const char* msg_type = "";
            switch (level)
            {
                case MSG_TRACE:     msg_type = MSG_TRACE_TEXT;  break;
                case MSG_INFO:      msg_type = MSG_INFO_TEXT;   break;
                case MSG_WARNING:   msg_type = MSG_WARNING_TEXT;break;
                case MSG_ERROR:     msg_type = MSG_ERROR_TEXT;  break;
                case MSG_FATAL:     msg_type = MSG_FATAL_TEXT;  break;
                default:            msg_type = "??UNKNOWN??";   break;
            }
            fprintf(current_logger->stream, "<%s>\t[%s]\t", time_str, msg_type);
        }

        va_list tmp_vlist = {};
        va_copy(tmp_vlist, vlist);
        vfprintf(current_logger->stream, format, tmp_vlist);

        fputc('\n', current_logger->stream);
    }

    va_end(vlist);

    #undef COLORED_CASE
    #undef ALL_CASES
}

void log_pause(void)    {paused = 1;}

void log_resume(void)   {paused = 0;}

void log_stop(void)
{
    for (size_t i = 0; i < loggers_count_; i++)
    {
        logger* current_logger = loggers_[i];
        if (current_logger->settings_mask & LGS_USE_HTML)
            fputs("</pre></body>\n", current_logger->stream);
        if (!(current_logger->settings_mask & LGS_KEEP_OPEN))
            fclose(current_logger->stream);
        free(current_logger);
    }
}
