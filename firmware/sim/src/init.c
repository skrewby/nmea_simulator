#include "bsp/init.h"
#include "state.h"
#include "ws_server.h"

#include <stdint.h>
#include <time.h>

void bsp_init(void) {
    state_init();
    ws_server_start();
}

void bsp_delay(uint32_t ms) {
    struct timespec ts = {
        .tv_sec = ms / 1000,
        .tv_nsec = (long)(ms % 1000) * 1000000L,
    };
    nanosleep(&ts, NULL);
}
