#include "bsp/irq.h"

#include "hal.h"

void bsp_disable_irq(void) { __disable_irq(); }

void bsp_enable_irq(void) { __enable_irq(); }
