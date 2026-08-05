#pragma once

#include <stdint.h>

typedef enum {
    HG_RES_DEVICE_INFORMATION,
    HG_RES_DEVICE_STATUS,
} Mercury_ResponseTag;

typedef struct {
    uint32_t serial_number;
    uint8_t fw_version_major;
    uint8_t fw_version_minor;
    uint8_t hw_revision;
} Mercury_Res_DeviceInformation;

typedef enum : uint8_t {
    HG_DEVSTATUS_OPERATIONAL = 0,
    HG_DEVSTATUS_INITIALIZATION = 1,
    HG_DEVSTATUS_ERROR = 2,
} Mercury_DeviceStatus;

typedef enum : uint8_t {
    HG_DEVMODE_MEMORY = 0,
    HG_DEVMODE_CONTROLLED = 1,
} Mercury_DeviceMode;

typedef struct {
    Mercury_DeviceStatus status;
    Mercury_DeviceMode mode;
} Mercury_Res_DeviceStatus;

typedef struct {
    Mercury_ResponseTag tag;

    union {
        Mercury_Res_DeviceInformation device_information;
        Mercury_Res_DeviceStatus device_status;
    } data;
} Mercury_Response;

void hg_send_response(Mercury_Response *res);
