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

#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu_fwd.h"

typedef struct {
  uint16_t size;
  char *memory;
} z80_memory_t;

int memory_init(cpu_t *cpu, uint16_t);
int memory_destroy(cpu_t *cpu);

uint16_t memory_get_size(cpu_t *cpu);

int memory_load(cpu_t *cpu, const uint8_t *buffer, size_t length);
int memory_load_at(cpu_t *cpu, const uint8_t *buffer, size_t length,
                   uint16_t offset);

int memory_set(cpu_t *cpu, uint16_t, uint8_t);
uint8_t memory_get(cpu_t *cpu, uint16_t);

#endif
