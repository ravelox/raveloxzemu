#ifndef CLOCK_H
#define CLOCK_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    uint32_t delay; // Artificial delay
    uint32_t t; // Number of remaining t-states
} z80_clock_t;

int clock_init(uint32_t delay);
int clock_destroy(void);
int clock_available(void);
int clock_delay(void);
int clock_has_t_state(void);
#endif