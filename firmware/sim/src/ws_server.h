#pragma once

#include "state.h"
#include <stddef.h>

void ws_server_start();
void ws_server_stop(void);

void ws_server_publish_gpio(const char *id, State_GPIO *gpio);
void ws_server_publish_serial(const char *id, const char *buf, uint16_t len);
void ws_server_publish_can(uint32_t can_id, const uint8_t *can_data, size_t can_data_len);
void ws_server_publish_full_state(State *state);
