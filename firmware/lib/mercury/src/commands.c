#include "mercury/commands.h"
#include "cobs.h"
#include "mercury_internal.h"
#include "packet.h"
#include <stddef.h>
#include <stdint.h>

void hg_send_command(Mercury_Command *cmd) {
    uint8_t buffer[ENCODED_PACKET_BUF_MAX_SIZE];
    size_t len = 0;

    switch (cmd->tag) {
    case HG_CMD_DEVICE_INFORMATION:
        len = setup_encoded_packet(CMDID_DEVICE_INFORMATION, NULL, 0, buffer);
        break;
    case HG_CMD_DEVICE_STATUS:
        len = setup_encoded_packet(CMDID_DEVICE_STATUS, NULL, 0, buffer);
        break;
    }

    transmit_fn(buffer, len);
}

Mercury_ParseResult parse_command(Packet *pkt, Mercury_Command *cmd) {
    switch (pkt->cmd_id) {
    case CMDID_DEVICE_INFORMATION:
        cmd->tag = HG_CMD_DEVICE_INFORMATION;
        return HG_RESULT_MESSAGE;
    case CMDID_DEVICE_STATUS:
        cmd->tag = HG_CMD_DEVICE_STATUS;
        return HG_RESULT_MESSAGE;
    default:
        return HG_RESULT_UNKNOWN_CMDID;
    }
}
