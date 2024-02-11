#include "log.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include "./globals.h"

#define log_va(type)             \
    if (is_silent(type)) return; \
    va_list args;                \
    va_start(args, format);      \
    log(type, format, args);     \
    va_end(args);

global_internal log_level_t log_level = LOG_LEVEL_WARNING;
global_internal char LOG_LEVEL_NAME[6] = {'S', 'E', 'W', 'I', 'D', 'V'};

global_internal bool is_silent(log_level_t level) {
    return log_level == LOG_LEVEL_SILENT || level > log_level;
}

global_internal void log(log_level_t level, const char *format, va_list args,
                         ...) {
    printf("[%c] ", LOG_LEVEL_NAME[level]);
    vprintf(format, args);
    printf("\n");
}

log_level_t log_get_level(void) { return log_level; }

void log_set_level(log_level_t level) { log_level = level; }

void log_verbose(const char *format, ...) { log_va(LOG_LEVEL_VERBOSE) }

void log_debug(const char *format, ...) { log_va(LOG_LEVEL_DEBUG) }

void log_info(const char *format, ...) { log_va(LOG_LEVEL_INFO) }

void log_warning(const char *format, ...) { log_va(LOG_LEVEL_WARNING) }

void log_error(const char *format, ...) { log_va(LOG_LEVEL_ERROR) }

