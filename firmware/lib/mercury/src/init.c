#include "mercury/init.h"
#include "mercury_internal.h"

Mercury_State state;
Mercury_TransmitFn transmit_fn;

void hg_init(const Mercury_Init *init) {
    state.sequence = 0;
    transmit_fn = init->transmit;
}
