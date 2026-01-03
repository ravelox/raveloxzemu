#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clock.h"

z80_clock_t *z80_clock;

int clock_init(uint32_t delay) {
  z80_clock = (z80_clock_t *)malloc(sizeof(z80_clock_t));
  if (!z80_clock) {
    fprintf(stderr, "Cannot allocate memory for clock\n");
    return -1;
  }

  z80_clock->delay = delay;
  z80_clock->t = 0;
  return 0;
}

int clock_destroy(void) {
  if (z80_clock)
    free(z80_clock);
  return 0;
}

int clock_available(void) { return (z80_clock != NULL); }

// Fake delay function
int clock_delay(void) {

  // If delay is 0, wait until enter key is pressed
  if (z80_clock->delay == 0) {
    int input_char = getc(stdin);

    while (1) {
      if (input_char == 0x0a)
        return 0;
      if (input_char == 'q')
        return -1;
    }
  } else {
    for (uint32_t i = 0; i < z80_clock->delay; i++) {
      continue;
    }
  }

  return 0;
}

int clock_has_t_state(void) { return (z80_clock->t > 0); }
