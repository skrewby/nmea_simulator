#include "bsp/init.h"
#include "bsp/led.h"
#include "bsp/time.h"
#include "log.h"
#include "version.h"

int main(void) {
    bsp_init();

    log_write(INFO, "======================= NMEA Simulator =======================");
    log_write(INFO, "Version: %s", FIRMWARE_VERSION);

    while (1) {
        bsp_led_toggle(BSP_LED_STATUS);
        bsp_delay(1000);
    }

    return 0;
}
