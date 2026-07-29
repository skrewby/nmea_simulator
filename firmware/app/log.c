#include "log.h"
#include "bsp/serial.h"
#include <stdarg.h>
#include <stdio.h>

#define SERIAL_PORT BSP_SERIAL_LOG

static LOG_LEVEL log_level = INFO;

void log_write(LOG_LEVEL level, const char *fmt, ...) {
    if (level < log_level) {
        return;
    }

    char buf[128];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (len < 0) {
        return;
    } else if ((size_t)len >= sizeof(buf)) {
        len = sizeof(buf) - 1;
    }

    switch (level) {
    case ERROR:
        bsp_serial_write(SERIAL_PORT, "[!] ", 4);
        break;
    default:
        break;
    }

    bsp_serial_write(SERIAL_PORT, buf, len);
    bsp_serial_write(SERIAL_PORT, "\r\n", 2);
}

void log_set_level(LOG_LEVEL level) { log_level = level; }
