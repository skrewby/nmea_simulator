#pragma once

#include <stdint.h>

typedef enum {
    NMEA_POSITION,
    NMEA_COGSOG,
    NMEA_ENVIRONMENTAL_PARAMETERS,
    NMEA_TEMPERATURE,
    NMEA_VESSEL_HEADING,
    NMEA_TAG_COUNT,
} NMEA2000_MessageTag;

typedef enum : uint8_t {
    NMEA_DIRREF_TRUE = 0,
    NMEA_DIRREF_MAGNETIC = 1,
    NMEA_DIRREF_ERROR = 2,
} NMEA2000_DirectionReference;

typedef struct {
    uint8_t sid;
    uint8_t cog_reference; // 0 = true, 1 = magnetic
    float cog;             // radians
    float sog;             // m/s
} NMEA2000_MSG_CogSog;

typedef struct {
    float latitude;  // degrees
    float longitude; // degrees
} NMEA2000_MSG_Position;

typedef struct {
    uint8_t sid;
    uint8_t temperature_source;
    uint8_t humidity_source;
    float temperature;             // K
    float humidity;                // %
    uint32_t atmospheric_pressure; // Pa
} NMEA2000_MSG_EnvironmentalParameters;

typedef struct {
    uint8_t sid;
    uint8_t instance;
    uint8_t source;
    float actual_temperature; // K
    float set_temperature;    // K
} NMEA2000_MSG_Temperature;

typedef struct {
    uint8_t sid;
    float heading;   // radians
    float deviation; // radians
    float variation; // radians
    NMEA2000_DirectionReference reference;
} NMEA2000_MSG_VesselHeading;

typedef struct {
    NMEA2000_MessageTag tag;

    union {
        NMEA2000_MSG_CogSog cog_sog;
        NMEA2000_MSG_Position position;
        NMEA2000_MSG_EnvironmentalParameters environmental_parameters;
        NMEA2000_MSG_Temperature temperature;
        NMEA2000_MSG_VesselHeading vessel_heading;
    } data;
} NMEA2000_Message;

void nmea2000_message_send(const NMEA2000_Message *message);

uint8_t nmea2000_message_priority(NMEA2000_MessageTag tag);

uint32_t nmea2000_message_period(NMEA2000_MessageTag tag);
