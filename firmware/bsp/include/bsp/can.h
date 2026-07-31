#pragma once

#include <stdint.h>

uint32_t bsp_can_transmit(uint32_t id, const uint8_t data[8]);
