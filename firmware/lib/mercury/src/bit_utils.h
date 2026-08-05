#pragma once

#include <stddef.h>
#include <stdint.h>

static inline void write_u32(uint32_t val, uint8_t *buf, size_t offset) {
    buf[0 + offset] = val & 0xFF;
    buf[1 + offset] = (val >> 8) & 0xFF;
    buf[2 + offset] = (val >> 16) & 0xFF;
    buf[3 + offset] = (val >> 24) & 0xFF;
}

static inline uint32_t read_u32(const uint8_t *buf, size_t offset) {
    return (uint32_t)buf[0 + offset] | ((uint32_t)buf[1 + offset] << 8) |
           ((uint32_t)buf[2 + offset] << 16) | ((uint32_t)buf[3 + offset] << 24);
}

static inline void write_u16(uint16_t val, uint8_t *buf, size_t offset) {
    buf[0 + offset] = val & 0xFF;
    buf[1 + offset] = (val >> 8) & 0xFF;
}

static inline uint16_t read_u16(const uint8_t *buf, size_t offset) {
    return (uint16_t)((uint16_t)buf[0 + offset] | ((uint16_t)buf[1 + offset] << 8));
}
