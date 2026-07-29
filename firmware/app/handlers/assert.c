#include "bsp/irq.h"

#include <assert.h>

void __assert_func(const char *file, int line, const char *func, const char *expr) {
    (void)file;
    (void)line;
    (void)func;
    (void)expr;

    bsp_disable_irq();

// The plan is to provide two different assert handlers depending if we're in Debug
// or Release mode. Since asserts will be enabled for Release, we want to make
// it so that the program instead recovers to a safe known state
#ifdef FW_ASSERT_DEBUG
    while (1) {
    }
#else
    while (1) {
    }
#endif
}
