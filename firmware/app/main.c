#include "bsp/can.h"
#include "bsp/init.h"
#include "bsp/led.h"
#include "bsp/time.h"
#include "log.h"
#include "nmea2000/init.h"
#include "nmea2000/message.h"
#include "version.h"
#include <assert.h>
#include <stdint.h>

// Used to keep track of milliseconds since the last message of its type was sent
static uint32_t MESSAGE_TIME[] = {
    [NMEA_POSITION] = 0,    [NMEA_COGSOG] = 0,         [NMEA_ENVIRONMENTAL_PARAMETERS] = 0,
    [NMEA_TEMPERATURE] = 0, [NMEA_VESSEL_HEADING] = 0,
};
static_assert(sizeof(MESSAGE_TIME) / sizeof(MESSAGE_TIME[0]) == NMEA_TAG_COUNT,
              "Added a NMEA_TAG without adding its time data");

static void nmea_transmit(uint32_t id, const uint8_t data[8]) {
    uint32_t error = bsp_can_transmit(id, data);
    if (error != 0) {
        log_write(ERROR, "NMEA CAN Error: %d", (int)error);
    }
}

static void register_device() {
    NMEA2000_DeviceFunction function = {
        .device_class = NAVIGATION,
        .function = OWNSHIP_POSITION_GNSS,
    };

    NMEA2000_DeviceDetails details = {
        .id = 120,
        .manufacturer_code = FURUNO,
        .device_instance_lower = 0,
        .device_instance_upper = 0,
        .device_function = function,
        .system_instance = 0,
        .industry_code = MARINE,
        .arbitrary_address_capable = 1,
    };

    uint8_t address = nmea2000_device_register(&details);

    log_write(INFO, "Device registered (address: %d)", address);
}

static void send_cogsog() {
    NMEA2000_Message msg = {
        .tag = NMEA_COGSOG,
        .data = {
            .cog_sog = {
                .sid = 1,
                .cog_reference = 0,
                .cog = 0.02,
                .sog = 2.78,
            }
        },
    };

    nmea2000_message_send(&msg);
}

static void send_position() {
    NMEA2000_Message msg = {
        .tag = NMEA_POSITION,
        .data = {
            .position = {
                .latitude = 12.3,
                .longitude = 2.41,
            }
        },
    };

    nmea2000_message_send(&msg);
}

static void send_env_parameters() {
    NMEA2000_Message msg = {
        .tag = NMEA_ENVIRONMENTAL_PARAMETERS,
        .data = {
            .environmental_parameters = {
                .sid = 1,
                .temperature_source = 2,
                .humidity_source = 1,
                .temperature = 270,
                .humidity = 79,
                .atmospheric_pressure = 5400,
            }
        },
    };

    nmea2000_message_send(&msg);
}

static void send_temperature() {
    NMEA2000_Message msg = {
        .tag = NMEA_TEMPERATURE,
        .data = {
            .temperature = {
                .sid = 1,
                .instance = 2,
                .source = 1,
                .actual_temperature = 280,
                .set_temperature = 290,
            }
        },
    };

    nmea2000_message_send(&msg);
}

static void send_vessel_heading() {
    NMEA2000_Message msg = {
        .tag = NMEA_VESSEL_HEADING,
        .data = {
            .vessel_heading = {
                .sid = 1,
                .heading = 0.70,
                .deviation = 0.001,
                .variation = 0.01,
                .reference = NMEA_DIRREF_MAGNETIC,
            }
        },
    };

    nmea2000_message_send(&msg);
}

static void send_messages(uint32_t dt) {
    for (NMEA2000_MessageTag tag = 0; tag < NMEA_TAG_COUNT; ++tag) {
        MESSAGE_TIME[tag] += dt;
        if (MESSAGE_TIME[tag] >= nmea2000_message_period(tag)) {
            switch (tag) {
            case NMEA_POSITION:
                send_position();
                break;
            case NMEA_COGSOG:
                send_cogsog();
                break;
            case NMEA_ENVIRONMENTAL_PARAMETERS:
                send_env_parameters();
                break;
            case NMEA_TEMPERATURE:
                send_temperature();
                break;
            case NMEA_VESSEL_HEADING:
                send_vessel_heading();
                break;
            case NMEA_TAG_COUNT:
                return;
            }

            MESSAGE_TIME[tag] = 0;
        }
    }
}

int main(void) {
    bsp_init();

    NMEA2000_Init nmea_init_info = {
        .transmit = nmea_transmit,
    };
    nmea2000_init(&nmea_init_info);

    log_write(INFO, "======================= NMEA Simulator =======================");
    log_write(INFO, "Version: %s", FIRMWARE_VERSION);

    register_device();

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
