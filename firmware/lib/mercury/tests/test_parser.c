#include "cobs.h"
#include "crc.h"
#include "mercury/init.h"
#include "mercury/parser.h"
#include "mercury_internal.h"
#include "packet.h"
#include "unity.h"

static Mercury_Message message;

void transmit(const uint8_t *data, size_t len) {
    (void)data;
    (void)len;
}

static Mercury_Init init = {
    .transmit = transmit,
};

void setUp(void) { hg_init(&init); }

void tearDown(void) {}

static size_t build_packet(Packet *packet, bool corrupt_crc,
                           uint8_t encoded[ENCODED_PACKET_BUF_MAX_SIZE]) {
    uint8_t packet_buffer[PACKET_BUF_MAX_SIZE];
    size_t packet_len = serialize_packet(packet, packet_buffer);

    uint16_t crc = crc16(packet_buffer, packet_len);
    if (corrupt_crc) {
        crc ^= 0xFFFF;
    }
    packet_buffer[packet_len] = crc & 0xFF;
    packet_buffer[packet_len + 1] = (crc >> 8) & 0xFF;
    packet_len += 2;

    return cobs_encode(packet_buffer, packet_len, encoded);
}

static Mercury_ParseResult process(const uint8_t *encoded, size_t encoded_len) {
    Mercury_ParseResult result = hg_process(0x00, &message);
    TEST_ASSERT_EQUAL(HG_RESULT_PROCESSING_COBS, result);

    for (size_t i = 0; i < encoded_len; i++) {
        result = hg_process(encoded[i], &message);
        TEST_ASSERT_EQUAL(HG_RESULT_PROCESSING_COBS, result);
    }

    return hg_process(0x00, &message);
}

void test_crc_invalid() {
    Packet packet = {
        .version = VERSION,
        .sequence = 0,
        .cmd_id = CMDID_DEVICE_INFORMATION,
        .length = 0,
    };

    uint8_t encoded[ENCODED_PACKET_BUF_MAX_SIZE];
    size_t encoded_len = build_packet(&packet, true, encoded);

    TEST_ASSERT_EQUAL(HG_RESULT_CRC_INVALID, process(encoded, encoded_len));
}

void test_unknown_cmdid() {
    Packet packet = {
        .version = VERSION,
        .sequence = 0,
        .cmd_id = 0x0000,
        .length = 0,
    };

    uint8_t encoded[ENCODED_PACKET_BUF_MAX_SIZE];
    size_t encoded_len = build_packet(&packet, false, encoded);

    TEST_ASSERT_EQUAL(HG_RESULT_UNKNOWN_CMDID, process(encoded, encoded_len));
}

void test_unknown_response_cmdid() {
    Packet packet = {
        .version = VERSION,
        .sequence = 0,
        .cmd_id = 0x0000 | RES_BIT,
        .length = 0,
    };

    uint8_t encoded[ENCODED_PACKET_BUF_MAX_SIZE];
    size_t encoded_len = build_packet(&packet, false, encoded);

    TEST_ASSERT_EQUAL(HG_RESULT_UNKNOWN_CMDID, process(encoded, encoded_len));
}

void test_packet_invalid() {
    uint8_t packet_buffer[PACKET_BUF_MIN_SIZE] = {
        VERSION,
        0,
        CMDID_DEVICE_INFORMATION & 0xFF,
        (CMDID_DEVICE_INFORMATION >> 8) & 0xFF,
        DATA_MAX_LEN,
        0,
        0,
    };

    uint8_t encoded[ENCODED_PACKET_BUF_MAX_SIZE];
    size_t encoded_len = cobs_encode(packet_buffer, sizeof(packet_buffer), encoded);

    TEST_ASSERT_EQUAL(HG_RESULT_PACKET_INVALID, process(encoded, encoded_len));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_crc_invalid);
    RUN_TEST(test_unknown_cmdid);
    RUN_TEST(test_unknown_response_cmdid);
    RUN_TEST(test_packet_invalid);

    return UNITY_END();
}
