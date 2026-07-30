#include "bsp/serial.h"
#include "state.h"
#include "ws_server.h"

void bsp_serial_write(BSP_SERIAL serial, const char *buf, uint16_t len) {
    ws_server_publish_serial(state_serial_name(serial), buf, len);
}
