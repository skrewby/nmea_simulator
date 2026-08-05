#include "mercury/commands.h"
#include "mercury/init.h"
#include "mercury/parser.h"
#include "packet.h"
#include "unity.h"

static Mercury_Message message;

void transmit(const uint8_t *data, size_t len) {
    Mercury_ParseResult result = HG_RESULT_WAITING_COBS;

    for (size_t i = 0; i < len; i++) {
        result = hg_process(data[i], &message);
    }

    TEST_ASSERT_EQUAL(HG_RESULT_MESSAGE, result);
}

static Mercury_Init init = {
    .transmit = transmit,
};

void setUp(void) { hg_init(&init); }

void tearDown(void) {}

void test_device_information() {
    Mercury_Command command = {
        .tag = HG_CMD_DEVICE_INFORMATION,
    };

    hg_send_command(&command);
    TEST_ASSERT_EQUAL(HG_COMMAND, message.tag);
    Mercury_Command cmd = message.data.command;
    TEST_ASSERT_EQUAL_INT16(HG_CMD_DEVICE_INFORMATION, cmd.tag);
}

void test_device_status() {
    Mercury_Command command = {
        .tag = HG_CMD_DEVICE_STATUS,
    };

    hg_send_command(&command);
    TEST_ASSERT_EQUAL(HG_COMMAND, message.tag);
    Mercury_Command cmd = message.data.command;
    TEST_ASSERT_EQUAL_INT16(HG_CMD_DEVICE_STATUS, cmd.tag);
}

void test_overflow() {
    Mercury_ParseResult result = hg_process(0x00, &message);
    TEST_ASSERT_EQUAL(HG_RESULT_PROCESSING_COBS, result);

    for (size_t i = 0; i < PACKET_BUF_MAX_SIZE + 1; i++) {
        result = hg_process(0xAA, &message);
        TEST_ASSERT_EQUAL(HG_RESULT_PROCESSING_COBS, result);
    }

    result = hg_process(0xAA, &message);
    TEST_ASSERT_EQUAL(HG_RESULT_PACKET_TOO_LARGE, result);

    result = hg_process(0xBB, &message);
    TEST_ASSERT_EQUAL(HG_RESULT_WAITING_COBS, result);

    result = hg_process(0x00, &message);
    TEST_ASSERT_EQUAL(HG_RESULT_PROCESSING_COBS, result);

    result = hg_process(0x00, &message);
    TEST_ASSERT_EQUAL(HG_RESULT_PROCESSING_COBS, result);

    result = hg_process(0x00, &message);
    TEST_ASSERT_EQUAL(HG_RESULT_PROCESSING_COBS, result);

    Mercury_Command command = {
        .tag = HG_CMD_DEVICE_STATUS,
    };
    hg_send_command(&command);
    TEST_ASSERT_EQUAL(HG_COMMAND, message.tag);
    TEST_ASSERT_EQUAL_INT16(HG_CMD_DEVICE_STATUS, message.data.command.tag);

    Mercury_Command command2 = {
        .tag = HG_CMD_DEVICE_INFORMATION,
    };
    hg_send_command(&command2);
    TEST_ASSERT_EQUAL(HG_COMMAND, message.tag);
    TEST_ASSERT_EQUAL_INT16(HG_CMD_DEVICE_INFORMATION, message.data.command.tag);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_device_information);
    RUN_TEST(test_device_status);
    RUN_TEST(test_overflow);

    return UNITY_END();
}
