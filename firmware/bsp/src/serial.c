#include "bsp/serial.h"

#include "hal.h"
#include <assert.h>

static UART_HandleTypeDef *periph[] = {
    [BSP_SERIAL_LOG] = &huart2,
};

static_assert(sizeof(periph) / sizeof(periph[0]) == BSP_SERIAL_COUNT,
              "periph[] missing an entry for a BSP_SERIAL");

void bsp_serial_write(BSP_SERIAL serial, const char *buf, uint16_t len) {
    assert(serial < BSP_SERIAL_COUNT);

    HAL_UART_Transmit(periph[serial], (const uint8_t *)buf, len, HAL_MAX_DELAY);
}
