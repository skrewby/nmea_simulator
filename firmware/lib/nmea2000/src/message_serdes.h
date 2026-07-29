#pragma once
#include "nmea2000/message.h"
#include <stdint.h>

// ============================== 129025 - Position, Rapid Update =============================== //
uint32_t serialize_position(const NMEA2000_MSG_Position *msg, uint8_t data[8]);

// ============================== 129026 - COG & SOG, Rapid Update ============================== //
uint32_t serialize_cogsog(const NMEA2000_MSG_CogSog *msg, uint8_t data[8]);

// ============================= 130311 - Environmental Parameters ============================== //
uint32_t serialize_env_parameters(const NMEA2000_MSG_EnvironmentalParameters *msg, uint8_t data[8]);

// ==================================== 130312 - Temperature ==================================== //
uint32_t serialize_temperature(const NMEA2000_MSG_Temperature *msg, uint8_t data[8]);

// =================================== 127250 - Vessel Heading ================================== //
uint32_t serialize_vessel_heading(const NMEA2000_MSG_VesselHeading *msg, uint8_t data[8]);
