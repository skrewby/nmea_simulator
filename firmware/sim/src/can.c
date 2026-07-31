#include "bsp/can.h"
#include "ws_server.h"

uint32_t bsp_can_transmit(uint32_t id, const uint8_t data[8]) {
    ws_server_publish_can(id, data, 8);
    return 0;
}
