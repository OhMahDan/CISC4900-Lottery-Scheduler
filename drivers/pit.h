#ifndef PIT_H
#define PIT_H

#include <stdint.h>
#include "isr.h"

// Tick counter: Used for random number generator
extern uint32_t ticks;

// Sets the tick frequency for the PIT
void PIT_init(uint32_t frequency);
// Handler for when int 32 fires
void PIT_handler_c(registers_t *r);
#endif
