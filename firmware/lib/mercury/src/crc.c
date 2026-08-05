#include "crc.h"
#include <stddef.h>
#include <stdint.h>

#define CRC_POLYNOMIAL 0x1021

uint16_t crc16(const uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ CRC_POLYNOMIAL;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}
