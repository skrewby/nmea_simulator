#pragma once

#include <stdint.h>

typedef enum : uint8_t {
    TRACE = 0,
    INFO = 1,
    ERROR = 2,
} LOG_LEVEL;

void log_set_level(LOG_LEVEL level);
void log_write(LOG_LEVEL level, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
