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

#include "cpu.h"

int cpu_init(cpu_t *cpu, uint32_t delay, uint16_t memory_size) {
  if (!cpu)
    return -1;

  cpu->last_instruction[0] = '\0';
  cpu->last_mem_read = 0;
  cpu->last_mem_write = 0;
  cpu->last_mem_read_valid = false;
  cpu->last_mem_write_valid = false;
  cpu->interrupts_enabled = false;
  cpu->halted = false;
  cpu->interrupt_mode = 0;

  if (register_init(cpu) != 0)
    return -1;
  if (clock_init(cpu, delay) != 0)
    return -1;
  if (memory_init(cpu, memory_size) != 0)
    return -1;

  return 0;
}

void cpu_destroy(cpu_t *cpu) {
  if (!cpu)
    return;

  memory_destroy(cpu);
  clock_destroy(cpu);
  register_destroy(cpu);
}
