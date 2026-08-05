#pragma once

typedef enum {
    HG_CMD_DEVICE_INFORMATION,
    HG_CMD_DEVICE_STATUS,
} Mercury_CommandTag;

typedef struct {
    Mercury_CommandTag tag;
} Mercury_Command;

void hg_send_command(Mercury_Command *cmd);
