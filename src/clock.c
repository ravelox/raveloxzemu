/*
 * This file is part of raveloxzemu.
 *
 * Copyright (C) 2026 Dave Kelly
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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

  if (cpu->clock.delay == 0)
    return 0;

  for (uint32_t i = 0; i < cpu->clock.delay; i++) {
    continue;
  }

  return 0;
}

int clock_has_t_state(cpu_t *cpu) {
  if (!cpu)
    return 0;
  return (cpu->clock.t > 0);
}
