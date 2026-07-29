#pragma once

#include <stdint.h>

static __attribute__((unused)) uint16_t read_u16(const uint8_t data[8], int idx) {
    return (uint16_t)(data[idx] | (data[idx + 1] << 8));
}

static __attribute__((unused)) uint32_t read_u32(const uint8_t data[8], int idx) {
    return (uint32_t)(data[idx] | (data[idx + 1] << 8) | (data[idx + 2] << 16) |
                      (data[idx + 3] << 24));
}

static __attribute__((unused)) int16_t read_i16(const uint8_t data[8], int idx) {
    return (int16_t)(data[idx] | (data[idx + 1] << 8));
}

static __attribute__((unused)) int32_t read_i32(const uint8_t data[8], int idx) {
    return (int32_t)(data[idx] | (data[idx + 1] << 8) | (data[idx + 2] << 16) |
                     (data[idx + 3] << 24));
}
