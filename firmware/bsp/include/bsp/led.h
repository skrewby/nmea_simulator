#pragma once

#include <stdint.h>

typedef enum : uint8_t {
    BSP_LED_STATUS,
    BSP_LED_COUNT,
} BSP_LED;

void bsp_led_toggle(BSP_LED led);
