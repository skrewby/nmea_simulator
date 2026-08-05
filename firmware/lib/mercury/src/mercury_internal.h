#pragma once

#include "mercury/commands.h"
#include "mercury/init.h"
#include "mercury/parser.h"
#include "mercury/responses.h"
#include "packet.h"
#include <stdint.h>

#define CMDID_DEVICE_INFORMATION 0x0100
#define CMDID_DEVICE_STATUS      0x0101

typedef struct {
    uint8_t sequence;
} Mercury_State;

extern Mercury_State state;
extern Mercury_TransmitFn transmit_fn;

Mercury_ParseResult parse_command(Packet *pkt, Mercury_Command *cmd);

Mercury_ParseResult parse_response(Packet *pkt, Mercury_Response *res);
