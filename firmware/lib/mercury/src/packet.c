#include "packet.h"
#include "cobs.h"
#include "crc.h"
#include "mercury_internal.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

size_t serialize_packet(Packet *packet, uint8_t buffer[PACKET_BUF_MAX_SIZE]) {
    assert(packet);

    buffer[0] = packet->version;
    buffer[1] = packet->sequence;
    buffer[2] = packet->cmd_id & 0xFF;
    buffer[3] = (packet->cmd_id >> 8) & 0xFF;
    buffer[4] = packet->length;

    for (uint8_t i = 0; i < packet->length; i++) {
        buffer[DATA_START_IDX + i] = packet->data[i];
    }

    return DATA_START_IDX + packet->length;
}

bool deserialize_packet(uint8_t buffer[PACKET_BUF_MAX_SIZE], size_t len, Packet *packet) {
    assert(packet);

    if (len < PACKET_BUF_MIN_SIZE || len >= PACKET_BUF_MAX_SIZE) {
        return false;
    }

    packet->version = buffer[0];
    packet->sequence = buffer[1];
    packet->cmd_id = buffer[2] | (buffer[3] << 8);
    packet->length = buffer[4];

    if (packet->length >= DATA_MAX_LEN) {
        return false;
    }

    if (packet->length > 0) {
        for (uint8_t i = 0; i < packet->length; i++) {
            packet->data[i] = buffer[DATA_START_IDX + i];
        }
    }

    size_t crc_idx = DATA_START_IDX + packet->length;
    packet->crc = buffer[crc_idx] | (buffer[crc_idx + 1] << 8);

    return true;
}

size_t setup_encoded_packet(uint16_t cmd_id, uint8_t *data, size_t len,
                            uint8_t buffer[ENCODED_PACKET_BUF_MAX_SIZE]) {
    Packet packet;
    packet.version = VERSION;
    packet.sequence = state.sequence++;
    packet.cmd_id = cmd_id;
    packet.length = len;

    if (len > 0 && data) {
        for (size_t i = 0; i < len; i++) {
            packet.data[i] = data[i];
        }
    }

    uint8_t packet_buffer[PACKET_BUF_MAX_SIZE];
    size_t packet_len = serialize_packet(&packet, packet_buffer);
    uint16_t crc = crc16(packet_buffer, packet_len);
    packet_buffer[packet_len] = crc & 0xFF;
    packet_buffer[packet_len + 1] = (crc >> 8) & 0xFF;
    packet_len += 2;

    buffer[0] = 0x00;
    size_t encoded_len = cobs_encode(packet_buffer, packet_len, buffer + 1);
    buffer[1 + encoded_len] = 0x00;

    return encoded_len + 2;
}
