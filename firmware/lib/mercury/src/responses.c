#include "mercury/responses.h"
#include "bit_utils.h"
#include "mercury/parser.h"
#include "mercury_internal.h"
#include "packet.h"
#include <stddef.h>
#include <stdint.h>

/**
 *  DEVICE INFORMATION
 **/
static size_t setup_device_information(const Mercury_Res_DeviceInformation *res,
                                       uint8_t buffer[ENCODED_PACKET_BUF_MAX_SIZE]) {
    uint8_t data[7];
    write_u32(res->serial_number, data, 0);
    data[4] = res->fw_version_major;
    data[5] = res->fw_version_minor;
    data[6] = res->hw_revision;

    size_t len = setup_encoded_packet(CMDID_DEVICE_INFORMATION | RES_BIT, data, 7, buffer);
    return len;
}

static void parse_device_information(const Packet *packet, Mercury_Res_DeviceInformation *res) {
    res->serial_number = read_u32(packet->data, 0);
    res->fw_version_major = packet->data[4];
    res->fw_version_minor = packet->data[5];
    res->hw_revision = packet->data[6];
}

/**
 *  DEVICE STATUS
 **/
static size_t setup_device_status(const Mercury_Res_DeviceStatus *res,
                                  uint8_t buffer[ENCODED_PACKET_BUF_MAX_SIZE]) {

    uint8_t data[2];
    data[0] = res->status;
    data[1] = res->mode;

    size_t len = setup_encoded_packet(CMDID_DEVICE_STATUS | RES_BIT, data, 2, buffer);
    return len;
}

static void parse_device_status(const Packet *packet, Mercury_Res_DeviceStatus *res) {
    res->status = packet->data[0];
    res->mode = packet->data[1];
}

/**
 *  PUBLIC IMPLEMENTATION
 **/
void hg_send_response(Mercury_Response *res) {
    uint8_t buffer[ENCODED_PACKET_BUF_MAX_SIZE];
    size_t len = 0;

    switch (res->tag) {
    case HG_RES_DEVICE_INFORMATION:
        len = setup_device_information(&res->data.device_information, buffer);
        break;
    case HG_RES_DEVICE_STATUS:
        len = setup_device_status(&res->data.device_status, buffer);
        break;
    }

    transmit_fn(buffer, len);
}

Mercury_ParseResult parse_response(Packet *packet, Mercury_Response *res) {
    switch (packet->cmd_id) {
    case (CMDID_DEVICE_INFORMATION | RES_BIT):
        res->tag = HG_RES_DEVICE_INFORMATION;
        parse_device_information(packet, &res->data.device_information);
        return HG_RESULT_MESSAGE;
    case (CMDID_DEVICE_STATUS | RES_BIT):
        res->tag = HG_RES_DEVICE_STATUS;
        parse_device_status(packet, &res->data.device_status);
        return HG_RESULT_MESSAGE;
    default:
        return HG_RESULT_UNKNOWN_CMDID;
    }
}
