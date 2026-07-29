#include "message_serdes.h"
#include "nmea2000/init.h"
#include "nmea2000/message.h"
#include <stdint.h>

static uint32_t PGN_ADDRESS_CLAIM_PRIORITY = 6;
static uint32_t PGN_ADDRESS_CLAIM_PF = 0xEE;
static uint32_t DESTINATION_GLOBAL = 0xFF;

static NMEA2000_CanTransmitFn transmit;
static uint8_t device_address = 0;

static uint64_t pack_name(const NMEA2000_DeviceDetails *details) {
    uint64_t n = 0;

    n |= (uint64_t)details->id & 0x1FFFFF;
    n |= (uint64_t)(details->manufacturer_code & 0x7FF) << 21;
    n |= (uint64_t)(details->device_instance_lower & 0x07) << 32;
    n |= (uint64_t)(details->device_instance_upper & 0x1F) << 35;
    n |= (uint64_t)(details->device_function.function) << 40;
    n |= (uint64_t)(details->device_function.device_class & 0x7F) << 49;
    n |= (uint64_t)(details->system_instance & 0x0F) << 56;
    n |= (uint64_t)(details->industry_code & 0x07) << 60;
    n |= (uint64_t)(details->arbitrary_address_capable) << 63;

    return n;
}

static void claim_address(const NMEA2000_DeviceDetails *details, uint8_t address) {
    uint64_t packed_name = pack_name(details);

    uint32_t id = (PGN_ADDRESS_CLAIM_PRIORITY << 26) | (PGN_ADDRESS_CLAIM_PF << 16) |
                  (DESTINATION_GLOBAL << 8) | address;

    uint8_t data[8];
    for (int i = 0; i < 8; ++i) {
        data[i] = packed_name >> (i * 8);
    }

    device_address = address;
    transmit(id, data);
}

uint8_t nmea2000_device_register(const NMEA2000_DeviceDetails *details) {
    uint8_t address = details->id & 252;

    claim_address(details, address);

    return address;
}

void nmea2000_init(const NMEA2000_Init *can) { transmit = can->transmit; }

void nmea2000_message_send(const NMEA2000_Message *message) {
    uint8_t buffer[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    uint32_t pgn = 0;
    switch (message->tag) {
    case NMEA_POSITION:
        pgn = serialize_position(&message->data.position, buffer);
        break;
    case NMEA_COGSOG:
        pgn = serialize_cogsog(&message->data.cog_sog, buffer);
        break;
    case NMEA_ENVIRONMENTAL_PARAMETERS:
        pgn = serialize_env_parameters(&message->data.environmental_parameters, buffer);
        break;
    case NMEA_TEMPERATURE:
        pgn = serialize_temperature(&message->data.temperature, buffer);
        break;
    case NMEA_VESSEL_HEADING:
        pgn = serialize_vessel_heading(&message->data.vessel_heading, buffer);
        break;
    case NMEA_TAG_COUNT:
        return;
    }

    uint8_t priority = nmea2000_message_priority(message->tag);
    uint32_t id = (priority << 26) | (pgn << 8) | device_address;

    transmit(id, buffer);
}
