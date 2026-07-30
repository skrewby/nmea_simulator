#pragma once

#include "bsp/led.h"
#include "bsp/serial.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool high;
} State_GPIO;

typedef struct {
    State_GPIO leds[BSP_LED_COUNT];
} State;

void state_init(void);

void state_toggle_led(BSP_LED led);

const char *state_led_name(BSP_LED led);
const char *state_serial_name(BSP_SERIAL serial);
State *state_get(void);
