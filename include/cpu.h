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

#ifndef CPU_H
#define CPU_H

#include <stdbool.h>
#include <stdint.h>

#include "clock.h"
#include "memory.h"
#include "register.h"

struct cpu {
  z80_clock_t clock;
  z80_memory_t memory;
  z80_register_t registers[REG_COUNT];
  z80_register_t alt_registers[REG_COUNT];
  char last_instruction[64];
  uint16_t last_mem_read;
  uint16_t last_mem_write;
  bool last_mem_read_valid;
  bool last_mem_write_valid;
  bool interrupts_enabled;
  bool halted;
  uint8_t interrupt_mode;
};

int cpu_init(cpu_t *cpu, uint32_t delay, uint16_t memory_size);
void cpu_destroy(cpu_t *cpu);

#endif
