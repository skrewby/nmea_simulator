#include "state.h"
#include "ws_server.h"

static State state;

static const char *LED_NAMES[] = {
    "Status",
};

static const char *SERIAL_NAMES[] = {
    "Log",
};

void state_init(void) {}

void state_toggle_led(BSP_LED led) {
    state.leds[led].high = !state.leds[led].high;
    ws_server_publish_gpio(state_led_name(led), &state.leds[led]);
}

const char *state_led_name(BSP_LED led) { return LED_NAMES[led]; }

const char *state_serial_name(BSP_SERIAL serial) { return SERIAL_NAMES[serial]; }

State *state_get(void) { return &state; }
