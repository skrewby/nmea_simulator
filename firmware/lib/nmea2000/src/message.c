#include "message_serdes.h"
#include <assert.h>
#include <math.h>
#include <stdint.h>

typedef struct {
    uint8_t priority;
    uint16_t period_ms;
} NMEA2000_MessageMeta;

static const NMEA2000_MessageMeta MESSAGE_META[] = {
    [NMEA_POSITION] = { .priority = 2,  .period_ms = 100 },
    [NMEA_COGSOG] = { .priority = 2,  .period_ms = 250 },
    [NMEA_ENVIRONMENTAL_PARAMETERS] = { .priority = 5,  .period_ms = 500 },
    [NMEA_TEMPERATURE] = { .priority = 5, .period_ms = 2000 },
    [NMEA_VESSEL_HEADING] = { .priority = 2,  .period_ms = 100 },
};
static_assert(sizeof(MESSAGE_META) / sizeof(MESSAGE_META[0]) == NMEA_TAG_COUNT,
              "Added a NMEA_TAG without adding its relevant meta information");

// ============================================ PGNs ============================================ //
static uint32_t VESSEL_HEADING = 127250;
static uint32_t POSITION = 129025;
static uint32_t COG_SOG = 129026;
static uint32_t ENVIRONMENTAL_PARAMETERS = 130311;
static uint32_t TEMPERATURE = 130312;

// ========================================= UTILITIES ========================================== //
static __attribute__((unused)) uint16_t read_u16(const uint8_t data[8], int idx) {
    return (uint16_t)(data[idx] | (data[idx + 1] << 8));
}

static __attribute__((unused)) uint32_t read_u32(const uint8_t data[8], int idx) {
    return (uint32_t)(data[idx] | (data[idx + 1] << 8) | (data[idx + 2] << 16) |
                      (data[idx + 3] << 24));
}

static __attribute__((unused)) int16_t read_i16(const uint8_t data[8], int idx) {
    return (int16_t)(data[idx] | (data[idx + 1] << 8));
}

static __attribute__((unused)) int32_t read_i32(const uint8_t data[8], int idx) {
    return (int32_t)(data[idx] | (data[idx + 1] << 8) | (data[idx + 2] << 16) |
                     (data[idx + 3] << 24));
}

static void write_u16(uint8_t data[8], int idx, uint16_t val) {
    data[idx] = (uint8_t)(val);
    data[idx + 1] = (uint8_t)(val >> 8);
}

static void write_i16(uint8_t data[8], int idx, int16_t val) {
    write_u16(data, idx, (uint16_t)val);
}

static void write_u32(uint8_t data[8], int idx, uint32_t val) {
    data[idx] = (uint8_t)(val);
    data[idx + 1] = (uint8_t)(val >> 8);
    data[idx + 2] = (uint8_t)(val >> 16);
    data[idx + 3] = (uint8_t)(val >> 24);
}

static void write_i32(uint8_t data[8], int idx, int32_t val) {
    write_u32(data, idx, (uint32_t)val);
}

uint8_t nmea2000_message_priority(NMEA2000_MessageTag tag) { return MESSAGE_META[tag].priority; }

uint32_t nmea2000_message_period(NMEA2000_MessageTag tag) { return MESSAGE_META[tag].period_ms; }

// ============================== 129025 - Position, Rapid Update =============================== //
uint32_t serialize_position(const NMEA2000_MSG_Position *msg, uint8_t data[8]) {
    write_i32(data, 0, (int32_t)lroundf(msg->latitude / 1e-7f));
    write_i32(data, 4, (int32_t)lroundf(msg->longitude / 1e-7f));

    return POSITION;
}

// ============================== 129026 - COG & SOG, Rapid Update ============================== //
uint32_t serialize_cogsog(const NMEA2000_MSG_CogSog *msg, uint8_t data[8]) {
    data[0] = msg->sid;
    data[1] = msg->cog_reference & 0x03;
    write_i16(data, 2, (int16_t)lroundf(msg->cog / 1e-4f));
    write_i16(data, 4, (int16_t)lroundf(msg->sog / 1e-2f));

    return COG_SOG;
}

// ============================= 130311 - Environmental Parameters ============================== //
uint32_t serialize_env_parameters(const NMEA2000_MSG_EnvironmentalParameters *msg,
                                  uint8_t data[8]) {
    data[0] = msg->sid;
    data[1] = (msg->temperature_source << 2) | msg->humidity_source;
    write_i16(data, 2, (int16_t)lroundf(msg->temperature / 1e-2f));
    write_i16(data, 4, (int16_t)lroundf(msg->humidity / 4e-3f));
    write_i16(data, 6, (int16_t)(msg->atmospheric_pressure / 100));

    return ENVIRONMENTAL_PARAMETERS;
}

// ==================================== 130312 - Temperature ==================================== //
uint32_t serialize_temperature(const NMEA2000_MSG_Temperature *msg, uint8_t data[8]) {
    data[0] = msg->sid;
    data[1] = msg->instance;
    data[2] = msg->source;
    write_u16(data, 3, (uint16_t)lroundf(msg->actual_temperature / 1e-2f));
    write_u16(data, 5, (uint16_t)lroundf(msg->set_temperature / 1e-2f));

    return TEMPERATURE;
}

// =================================== 127250 - Vessel Heading ================================== //
uint32_t serialize_vessel_heading(const NMEA2000_MSG_VesselHeading *msg, uint8_t data[8]) {
    data[0] = msg->sid;
    write_i16(data, 1, (int16_t)lroundf(msg->heading / 1e-4f));
    write_i16(data, 3, (int16_t)lroundf(msg->deviation / 1e-4f));
    write_i16(data, 5, (int16_t)lroundf(msg->variation / 1e-4f));
    data[7] = msg->reference;

    return VESSEL_HEADING;
}
