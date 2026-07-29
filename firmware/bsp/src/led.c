#include "bsp/led.h"

#include "hal.h"
#include <assert.h>

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} LedGpio;

static const LedGpio leds[] = {
    [BSP_LED_STATUS] = {
        .port = LED_STATUS_GPIO_Port,
        .pin = LED_STATUS_Pin,
    },
};

static_assert(sizeof(leds) / sizeof(leds[0]) == BSP_LED_COUNT,
              "leds[] missing an entry for a BSP_LED");

void bsp_led_toggle(BSP_LED led) {
    assert(led < BSP_LED_COUNT);
    HAL_GPIO_TogglePin(leds[led].port, leds[led].pin);
}
