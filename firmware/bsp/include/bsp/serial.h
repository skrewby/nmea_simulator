#pragma once

#include <stdint.h>

typedef enum : uint8_t {
    BSP_SERIAL_LOG,
    BSP_SERIAL_COUNT,
} BSP_SERIAL;

void bsp_serial_write(BSP_SERIAL serial, const char *buf, uint16_t len);
