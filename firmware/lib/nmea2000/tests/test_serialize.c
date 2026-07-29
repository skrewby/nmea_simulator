#include "message_serdes.h"
#include "nmea2000/message.h"
#include "test_utils.h"
#include "unity.h"
#include <stdint.h>

void setUp(void) {}

void tearDown(void) {}

void test_serialize_position(void) {
    NMEA2000_MSG_Position msg = {
        .latitude = 34.56,
        .longitude = 172.44,
    };
    uint8_t data[8] = { 0 };
    uint32_t pgn = serialize_position(&msg, data);
    TEST_ASSERT_EQUAL_UINT32(129025, pgn);
    TEST_ASSERT_EQUAL_INT32(345600000, read_i32(data, 0));
    TEST_ASSERT_EQUAL_INT32(1724400000, read_i32(data, 4));

    msg.latitude = 90;
    msg.longitude = 180;
    pgn = serialize_position(&msg, data);
    TEST_ASSERT_EQUAL_UINT32(129025, pgn);
    TEST_ASSERT_EQUAL_INT32(900000000, read_i32(data, 0));
    TEST_ASSERT_EQUAL_INT32(1800000000, read_i32(data, 4));

    msg.latitude = -90;
    msg.longitude = -180;
    pgn = serialize_position(&msg, data);
    TEST_ASSERT_EQUAL_UINT32(129025, pgn);
    TEST_ASSERT_EQUAL_INT32(-900000000, read_i32(data, 0));
    TEST_ASSERT_EQUAL_INT32(-1800000000, read_i32(data, 4));
}

void test_serialize_cogsog(void) {
    NMEA2000_MSG_CogSog msg = {
        .sid = 12,
        .cog_reference = 1,
        .cog = 0.34,
        .sog = 5.67,
    };
    uint8_t data[8] = { 0 };
    uint32_t pgn = serialize_cogsog(&msg, data);
    TEST_ASSERT_EQUAL_UINT32(129026, pgn);
    TEST_ASSERT_EQUAL_UINT8(12, data[0]);
    TEST_ASSERT_EQUAL_UINT8(1, data[1]);
    TEST_ASSERT_EQUAL_UINT16(3400, read_u16(data, 2));
    TEST_ASSERT_EQUAL_UINT16(567, read_u16(data, 4));

    msg.sid = 252;
    msg.cog_reference = 2;
    msg.cog = 6.2831852;
    msg.sog = 655.32;
    pgn = serialize_cogsog(&msg, data);
    TEST_ASSERT_EQUAL_UINT32(129026, pgn);
    TEST_ASSERT_EQUAL_UINT8(252, data[0]);
    TEST_ASSERT_EQUAL_UINT8(2, data[1]);
    TEST_ASSERT_EQUAL_UINT16(62832, read_u16(data, 2));
    TEST_ASSERT_EQUAL_UINT16(65532, read_u16(data, 4));

    msg.sid = 0;
    msg.cog_reference = 0;
    msg.cog = 0;
    msg.sog = 0;
    pgn = serialize_cogsog(&msg, data);
    TEST_ASSERT_EQUAL_UINT32(129026, pgn);
    TEST_ASSERT_EQUAL_UINT8(0, data[0]);
    TEST_ASSERT_EQUAL_UINT8(0, data[1]);
    TEST_ASSERT_EQUAL_UINT16(0, read_u16(data, 2));
    TEST_ASSERT_EQUAL_UINT16(0, read_u16(data, 4));
}

void test_serialize_env_parameters(void) {
    NMEA2000_MSG_EnvironmentalParameters msg = {
        .sid = 123,
        .temperature_source = 3,
        .humidity_source = 1,
        .temperature = 230.5,
        .humidity = 79.42,
        .atmospheric_pressure = 101325,
    };
    uint8_t data[8] = { 0 };
    uint32_t pgn = serialize_env_parameters(&msg, data);
    TEST_ASSERT_EQUAL_UINT32(130311, pgn);
    TEST_ASSERT_EQUAL_UINT8(123, data[0]);
    TEST_ASSERT_EQUAL_UINT8(3, data[1] >> 2);
    TEST_ASSERT_EQUAL_UINT8(1, data[1] & 0x3);
    TEST_ASSERT_EQUAL_UINT16(23050, read_u16(data, 2));
    TEST_ASSERT_EQUAL_INT16(79420 / 4, read_i16(data, 4));
    TEST_ASSERT_EQUAL_UINT16(1013, read_u16(data, 6));

    msg.sid = 252;
    msg.temperature_source = 61;
    msg.humidity_source = 2;
    msg.temperature = 655.32;
    msg.humidity = 131.056;
    msg.atmospheric_pressure = 6553200;
    pgn = serialize_env_parameters(&msg, data);
    TEST_ASSERT_EQUAL_UINT32(130311, pgn);
    TEST_ASSERT_EQUAL_UINT8(252, data[0]);
    TEST_ASSERT_EQUAL_UINT8(61, data[1] >> 2);
    TEST_ASSERT_EQUAL_UINT8(2, data[1] & 0x3);
    TEST_ASSERT_EQUAL_UINT16(65532, read_u16(data, 2));
    TEST_ASSERT_EQUAL_INT16(131056 / 4, read_i16(data, 4));
    TEST_ASSERT_EQUAL_UINT16(65532, read_u16(data, 6));

    msg.sid = 0;
    msg.temperature_source = 0;
    msg.humidity_source = 0;
    msg.temperature = 0;
    msg.humidity = -131.068;
    msg.atmospheric_pressure = 0;
    pgn = serialize_env_parameters(&msg, data);
    TEST_ASSERT_EQUAL_UINT32(130311, pgn);
    TEST_ASSERT_EQUAL_UINT8(0, data[0]);
    TEST_ASSERT_EQUAL_UINT8(0, data[1] >> 2);
    TEST_ASSERT_EQUAL_UINT8(0, data[1] & 0x3);
    TEST_ASSERT_EQUAL_UINT16(0, read_u16(data, 2));
    TEST_ASSERT_EQUAL_INT16(-131068 / 4, read_i16(data, 4));
    TEST_ASSERT_EQUAL_UINT16(0, read_u16(data, 6));
}

void test_serialize_temperature(void) {
    NMEA2000_MSG_Temperature msg = {
        .sid = 32,
        .instance = 122,
        .source = 3,
        .actual_temperature = 297.15,
        .set_temperature = 300.22,
    };
    uint8_t data[8] = { 0 };
    uint32_t pgn = serialize_temperature(&msg, data);
    TEST_ASSERT_EQUAL_UINT32(130312, pgn);
    TEST_ASSERT_EQUAL_UINT8(32, data[0]);
    TEST_ASSERT_EQUAL_UINT8(122, data[1]);
    TEST_ASSERT_EQUAL_UINT8(3, data[2]);
    TEST_ASSERT_EQUAL_UINT16(29715, read_u16(data, 3));
    TEST_ASSERT_EQUAL_UINT16(30022, read_u16(data, 5));

    msg.sid = 252;
    msg.instance = 252;
    msg.source = 252;
    msg.actual_temperature = 655.32;
    msg.set_temperature = 655.32;
    pgn = serialize_temperature(&msg, data);
    TEST_ASSERT_EQUAL_UINT32(130312, pgn);
    TEST_ASSERT_EQUAL_UINT8(252, data[0]);
    TEST_ASSERT_EQUAL_UINT8(252, data[1]);
    TEST_ASSERT_EQUAL_UINT8(252, data[2]);
    TEST_ASSERT_EQUAL_UINT16(65532, read_u16(data, 3));
    TEST_ASSERT_EQUAL_UINT16(65532, read_u16(data, 5));

    msg.sid = 0;
    msg.instance = 0;
    msg.source = 0;
    msg.actual_temperature = 0;
    msg.set_temperature = 0;
    pgn = serialize_temperature(&msg, data);
    TEST_ASSERT_EQUAL_UINT32(130312, pgn);
    TEST_ASSERT_EQUAL_UINT8(0, data[0]);
    TEST_ASSERT_EQUAL_UINT8(0, data[1]);
    TEST_ASSERT_EQUAL_UINT8(0, data[2]);
    TEST_ASSERT_EQUAL_UINT16(0, read_u16(data, 3));
    TEST_ASSERT_EQUAL_UINT16(0, read_u16(data, 5));
}

void test_serialize_vessel_heading(void) {
    NMEA2000_MSG_VesselHeading msg = {
        .sid = 58,
        .heading = 2.32,
        .deviation = 0.002,
        .variation = 0.01,
        .reference = 1,
    };
    uint8_t data[8] = { 0 };
    uint32_t pgn = serialize_vessel_heading(&msg, data);
    TEST_ASSERT_EQUAL_UINT8(127250, pgn);
    TEST_ASSERT_EQUAL_UINT8(58, data[0]);
    TEST_ASSERT_EQUAL_UINT16(23200, read_u16(data, 1));
    TEST_ASSERT_EQUAL_INT16(20, read_i16(data, 3));
    TEST_ASSERT_EQUAL_INT16(100, read_i16(data, 5));
    TEST_ASSERT_EQUAL_UINT8(1, data[7] & 0x3);

    msg.sid = 252;
    msg.heading = 6.2831852;
    msg.deviation = 3.1415926;
    msg.variation = 3.1415926;
    msg.reference = 2;
    pgn = serialize_vessel_heading(&msg, data);
    TEST_ASSERT_EQUAL_UINT8(127250, pgn);
    TEST_ASSERT_EQUAL_UINT8(252, data[0]);
    TEST_ASSERT_EQUAL_UINT16(62832, read_u16(data, 1));
    TEST_ASSERT_EQUAL_INT16(31416, read_i16(data, 3));
    TEST_ASSERT_EQUAL_INT16(31416, read_i16(data, 5));
    TEST_ASSERT_EQUAL_UINT8(2, data[7] & 0x3);

    msg.sid = 0;
    msg.heading = 0;
    msg.deviation = -3.1415926;
    msg.variation = -3.1415926;
    msg.reference = 0;
    pgn = serialize_vessel_heading(&msg, data);
    TEST_ASSERT_EQUAL_UINT8(127250, pgn);
    TEST_ASSERT_EQUAL_UINT8(0, data[0]);
    TEST_ASSERT_EQUAL_UINT16(0, read_u16(data, 1));
    TEST_ASSERT_EQUAL_INT16(-31416, read_i16(data, 3));
    TEST_ASSERT_EQUAL_INT16(-31416, read_i16(data, 5));
    TEST_ASSERT_EQUAL_UINT8(0, data[7] & 0x3);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_serialize_position);
    RUN_TEST(test_serialize_cogsog);
    RUN_TEST(test_serialize_env_parameters);
    RUN_TEST(test_serialize_temperature);
    RUN_TEST(test_serialize_vessel_heading);

    return UNITY_END();
}
