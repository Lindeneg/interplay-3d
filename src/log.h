#ifndef INTERPLAY_LOG_H_
#define INTERPLAY_LOG_H_

typedef enum log_level {
    LOG_LEVEL_SILENT,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_VERBOSE
} log_level_t;

log_level_t log_get_level(void);

void log_set_level(log_level_t level);

void log_error(const char *format, ...);

void log_warning(const char *format, ...);

void log_info(const char *format, ...);

void log_debug(const char *format, ...);

void log_verbose(const char *format, ...);

#endif  // INTERPLAY_LOG_H_
