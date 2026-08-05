#pragma once

#include <stddef.h>
#include <stdint.h>

typedef void (*Mercury_TransmitFn)(const uint8_t *data, size_t len);

typedef struct {
    Mercury_TransmitFn transmit;
} Mercury_Init;

void hg_init(const Mercury_Init *init);
