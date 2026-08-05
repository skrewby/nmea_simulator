#pragma once

#include "mercury/commands.h"
#include "mercury/responses.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    HG_RESULT_WAITING_COBS,     // Waiting for 0x00 to announce new packet
    HG_RESULT_PROCESSING_COBS,  // Currently reading COBS encoded packet
    HG_RESULT_MESSAGE,          // Full message received
    HG_RESULT_CRC_INVALID,      // Full message received but CRC is invalid
    HG_RESULT_PACKET_TOO_LARGE, // COBS Packet is larger than the maximum, it will be destroyed
    HG_RESULT_PACKET_INVALID,   // The command was recognized but the packet was malformed
    HG_RESULT_UNKNOWN_CMDID,    // Unrecognized command or response
} Mercury_ParseResult;

typedef enum {
    HG_COMMAND,
    HG_RESPONSE,
} Mercury_MessageTag;

typedef struct {
    Mercury_MessageTag tag;

    union {
        Mercury_Command command;
        Mercury_Response response;
    } data;
} Mercury_Message;

Mercury_ParseResult hg_process(uint8_t byte, Mercury_Message *msg_out);
