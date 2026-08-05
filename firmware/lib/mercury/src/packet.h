#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define RES_BIT                     0x8000
#define VERSION                     1
#define DATA_MAX_LEN                10
#define DATA_START_IDX              5
#define PACKET_BUF_MIN_SIZE         7
#define PACKET_BUF_MAX_SIZE         (DATA_MAX_LEN + PACKET_BUF_MIN_SIZE)
#define ENCODED_PACKET_BUF_MAX_SIZE (DATA_MAX_LEN + PACKET_BUF_MIN_SIZE + 3)

typedef struct {
    uint8_t version;
    uint8_t sequence;
    uint16_t cmd_id;
    uint8_t length;
    uint8_t data[DATA_MAX_LEN];
    uint16_t crc;
} Packet;

/**
 *  Write the packet into a buffer of bytes. Does not include or compute CRC bytes
 *
 *  @param  packet  The packet to serialize
 *  @param  buffer  The buffer to write the bytes into
 *  @return         Number of bytes written to buffer
 **/
size_t serialize_packet(Packet *packet, uint8_t buffer[PACKET_BUF_MAX_SIZE]);

bool deserialize_packet(uint8_t buffer[PACKET_BUF_MAX_SIZE], size_t len, Packet *packet);

/**
 *  Creates a packet, serializes it, calculates the CRC and then encodes with COBS
 *
 *  @param  data    Data buffer of the packet
 *  @param  len     How many bytes to copy from the data buffer to the Packet data section
 *  @param  cmd_id  The Command ID to add to the packet
 *  @param  buffer  (Output) The buffer to write the encoded packet to
 **/
size_t setup_encoded_packet(uint16_t cmd_id, uint8_t *data, size_t len,
                            uint8_t buffer[ENCODED_PACKET_BUF_MAX_SIZE]);
