#include "bsp/irq.h"

void fault_handler(void) {
    bsp_disable_irq();

    while (1) {
    }
}
