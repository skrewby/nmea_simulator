#include "bsp/init.h"
#include "bsp/led.h"
#include "bsp/time.h"
#include "log.h"
#include "nmea.h"
#include "version.h"
#include <assert.h>
#include <stdint.h>

int main(void) {
    bsp_init();
    nmea_init();

    log_write(INFO, "======================= NMEA Simulator =======================");
    log_write(INFO, "Version: %s", FIRMWARE_VERSION);

    uint32_t delay_ms = 100;
    uint8_t update_led = 1000 / delay_ms;
    while (1) {
        send_messages(delay_ms);

        update_led--;
        if (update_led <= 0) {
            bsp_led_toggle(BSP_LED_STATUS);
            update_led = 1000 / delay_ms;
        }

        bsp_delay(delay_ms);
    }

    return 0;
}
