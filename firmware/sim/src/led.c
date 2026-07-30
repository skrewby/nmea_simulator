#include "bsp/led.h"
#include "state.h"

void bsp_led_toggle(BSP_LED led) { state_toggle_led(led); }
