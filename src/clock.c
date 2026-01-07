#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "clock.h"

int clock_init(cpu_t *cpu, uint32_t delay) {
  if (!cpu) {
    fprintf(stderr, "Cannot initialize clock\n");
    return -1;
  }

  cpu->clock.delay = delay;
  cpu->clock.t = 0;
  return 0;
}

int clock_destroy(cpu_t *cpu) {
  if (!cpu)
    return 0;
  cpu->clock.delay = 0;
  cpu->clock.t = 0;
  return 0;
}

int clock_available(cpu_t *cpu) { return (cpu != NULL); }

// Fake delay function
int clock_delay(cpu_t *cpu) {
  if (!cpu)
    return -1;

  // If delay is 0, wait until enter key is pressed
  if (cpu->clock.delay == 0) {
    int input_char = getc(stdin);

    while (1) {
      if (input_char == 0x0a)
        return 0;
      if (input_char == 'q')
        return -1;
    }
  } else {
    for (uint32_t i = 0; i < cpu->clock.delay; i++) {
      continue;
    }
  }

  return 0;
}

int clock_has_t_state(cpu_t *cpu) {
  if (!cpu)
    return 0;
  return (cpu->clock.t > 0);
}
