#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "cpu_fwd.h"

typedef struct {
    uint32_t delay; // Artificial delay
    uint32_t t; // Number of remaining t-states
} z80_clock_t;

int clock_init(cpu_t *cpu, uint32_t delay);
int clock_destroy(cpu_t *cpu);
int clock_available(cpu_t *cpu);
int clock_delay(cpu_t *cpu);
int clock_has_t_state(cpu_t *cpu);
#endif
