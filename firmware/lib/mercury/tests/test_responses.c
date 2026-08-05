#include "mercury/init.h"
#include "mercury/parser.h"
#include "mercury/responses.h"
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
    Mercury_Response response = {
        .tag = HG_RES_DEVICE_INFORMATION,
        .data.device_information = {
            .serial_number = 100457,
            .fw_version_major = 123,
            .fw_version_minor = 45,
            .hw_revision = 65,
        }
    };

    hg_send_response(&response);
    TEST_ASSERT_EQUAL(HG_RESPONSE, message.tag);
    Mercury_Response res = message.data.response;
    TEST_ASSERT_EQUAL_INT16(HG_RES_DEVICE_INFORMATION, res.tag);
    Mercury_Res_DeviceInformation msg = res.data.device_information;
    TEST_ASSERT_EQUAL_UINT32(100457, msg.serial_number);
    TEST_ASSERT_EQUAL_UINT8(123, msg.fw_version_major);
    TEST_ASSERT_EQUAL_UINT8(45, msg.fw_version_minor);
    TEST_ASSERT_EQUAL_UINT8(65, msg.hw_revision);
}

void test_device_status() {
    Mercury_Response response = {
        .tag = HG_RES_DEVICE_STATUS,
        .data.device_status = {
            .status = HG_DEVSTATUS_INITIALIZATION,
            .mode = HG_DEVMODE_CONTROLLED,
        }
    };

    hg_send_response(&response);
    TEST_ASSERT_EQUAL(HG_RESPONSE, message.tag);
    Mercury_Response res = message.data.response;
    TEST_ASSERT_EQUAL_INT16(HG_RES_DEVICE_STATUS, res.tag);
    Mercury_Res_DeviceStatus msg = res.data.device_status;
    TEST_ASSERT_EQUAL_UINT8(HG_DEVSTATUS_INITIALIZATION, msg.status);
    TEST_ASSERT_EQUAL_UINT8(HG_DEVMODE_CONTROLLED, msg.mode);
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

    Mercury_Response response = {
        .tag = HG_RES_DEVICE_INFORMATION,
        .data.device_information = {
            .serial_number = 100457,
            .fw_version_major = 123,
            .fw_version_minor = 45,
            .hw_revision = 65,
        }
    };

    hg_send_response(&response);
    TEST_ASSERT_EQUAL(HG_RESPONSE, message.tag);
    Mercury_Response res = message.data.response;
    TEST_ASSERT_EQUAL_INT16(HG_RES_DEVICE_INFORMATION, res.tag);

    Mercury_Response response2 = {
        .tag = HG_RES_DEVICE_INFORMATION,
        .data.device_information = {
            .serial_number = 100457,
            .fw_version_major = 123,
            .fw_version_minor = 45,
            .hw_revision = 65,
        }
    };

    hg_send_response(&response2);
    TEST_ASSERT_EQUAL(HG_RESPONSE, message.tag);
    Mercury_Response res2 = message.data.response;
    TEST_ASSERT_EQUAL_INT16(HG_RES_DEVICE_INFORMATION, res2.tag);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_device_information);
    RUN_TEST(test_device_status);
    RUN_TEST(test_overflow);

    return UNITY_END();
}
