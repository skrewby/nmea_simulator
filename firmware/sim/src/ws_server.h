#pragma once

#include "state.h"

void ws_server_start();
void ws_server_stop(void);

void ws_server_publish_gpio(const char *id, State_GPIO *gpio);
void ws_server_publish_full_state(State *state);
