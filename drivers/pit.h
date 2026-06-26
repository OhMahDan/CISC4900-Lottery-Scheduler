#ifndef PIT_H
#define PIT_H

#include <stdint.h>
#include "isr.h"

// Sets the tick frequency for the PIT
void PIT_init(uint32_t frequency);
// Handler for when int 32 fires
void PIT_handler_c(registers_t *r);
#endif
