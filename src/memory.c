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
#include "memory.h"

int memory_init(cpu_t *cpu, uint16_t memory_size) {
  if (!cpu)
    return -1;

  cpu->memory.size = memory_size;
  cpu->memory.memory = (char *)malloc(cpu->memory.size);

  if (!cpu->memory.memory) {
    fprintf(stderr, "Cannot allocate memory space: %u\n", cpu->memory.size);
    return -1;
  }

  memset(cpu->memory.memory, 0, cpu->memory.size);

  fprintf(stdout, "Memory allocated: %x\n", memory_size);
  return 0;
}

int memory_destroy(cpu_t *cpu) {
  if (!cpu)
    return 0;
  if (cpu->memory.memory)
    free(cpu->memory.memory);
  cpu->memory.memory = NULL;
  cpu->memory.size = 0;
  return 0;
}

uint16_t memory_get_size(cpu_t *cpu) {
  if (!cpu) {
    fprintf(stderr, "Memory not allocated\n");
    return 0;
  }
  return cpu->memory.size;
}

int memory_load(cpu_t *cpu, const uint8_t *buffer, size_t length) {
  return memory_load_at(cpu, buffer, length, 0);
}

int memory_load_at(cpu_t *cpu, const uint8_t *buffer, size_t length,
                   uint16_t offset) {
  if (!cpu || !cpu->memory.memory || !buffer)
    return -1;
  if ((size_t)offset + length > cpu->memory.size)
    return -1;

  memcpy(cpu->memory.memory + offset, buffer, length);
  return 0;
}

int memory_set(cpu_t *cpu, uint16_t address, uint8_t value) {
  if (!cpu)
    return -1;
  if (!cpu->memory.memory)
    return -1;

  cpu->memory.memory[address] = value;
  cpu->last_mem_write = address;
  cpu->last_mem_write_valid = true;
  return 0;
}

uint8_t memory_get(cpu_t *cpu, uint16_t address) {
  if (!cpu)
    return 0;
  if (!cpu->memory.memory)
    return 0;

  cpu->last_mem_read = address;
  cpu->last_mem_read_valid = true;
  return cpu->memory.memory[address];
}
