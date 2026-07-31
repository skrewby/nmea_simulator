#include "bsp/init.h"

#include "hal.h"

void bsp_init(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_CAN1_Init();
    HAL_CAN_Start(&hcan1);
}

void bsp_delay(uint32_t ms) { HAL_Delay(ms); }
