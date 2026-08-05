#include "mercury/parser.h"
#include "cobs.h"
#include "crc.h"
#include "mercury_internal.h"
#include "packet.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static uint8_t buffer[PACKET_BUF_MAX_SIZE + 1];
static uint8_t decoded_buffer[PACKET_BUF_MAX_SIZE];
static Packet packet;

static int idx = 0;
static bool packet_start = false;

static Mercury_ParseResult parse_packet(Mercury_Message *msg_out) {
    if (packet.cmd_id & RES_BIT) {
        msg_out->tag = HG_RESPONSE;
        return parse_response(&packet, &msg_out->data.response);
    } else {
        msg_out->tag = HG_COMMAND;
        return parse_command(&packet, &msg_out->data.command);
    }
}

Mercury_ParseResult hg_process(uint8_t byte, Mercury_Message *msg_out) {
    if (!packet_start) {
        if (byte != 0x00) {
            return HG_RESULT_WAITING_COBS;
        } else {
            packet_start = true;
            return HG_RESULT_PROCESSING_COBS;
        }
    } else if (byte == 0x00) {
        if (idx == 0) {
            return HG_RESULT_PROCESSING_COBS;
        }

        size_t len = cobs_decode(buffer, idx, decoded_buffer);
        idx = 0;
        packet_start = false;

        if (!deserialize_packet(decoded_buffer, len, &packet)) {
            return HG_RESULT_PACKET_INVALID;
        }

        if (crc16(decoded_buffer, len - 2) != packet.crc) {
            return HG_RESULT_CRC_INVALID;
        }

        return parse_packet(msg_out);
    }

    if (idx >= (int)sizeof(buffer)) {
        idx = 0;
        packet_start = false;
        return HG_RESULT_PACKET_TOO_LARGE;
    }

    buffer[idx++] = byte;

    return HG_RESULT_PROCESSING_COBS;
}
