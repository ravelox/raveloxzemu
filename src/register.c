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

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "memory.h"
#include "register.h"

static const uint8_t _reg_map[] = {REG_B, REG_C, REG_D, REG_E,
                                   REG_H, REG_L, REG_A};

int register_init(cpu_t *cpu) {
  size_t register_size = sizeof(z80_register_t) * REG_COUNT;

  if (!cpu)
    return -1;

  memset(cpu->registers, 0, register_size);
  memset(cpu->alt_registers, 0, register_size);

  return 0;
}

int register_destroy(cpu_t *cpu) {
  if (!cpu)
    return 0;

  memset(cpu->registers, 0, sizeof(cpu->registers));
  memset(cpu->alt_registers, 0, sizeof(cpu->alt_registers));
  return 0;
}

static int _register_value_set(z80_register_t *z80_registers, uint8_t index,
                               uint16_t value) {
  uint8_t real_index = index & REG_MASK;

  if (real_index >= REG_COUNT)
    return -1;

  if (index & HIGH_BYTE) {
    z80_registers[real_index].bytes.high = value;
  } else if (index & LOW_BYTE) {
    z80_registers[real_index].bytes.low = value;
  } else {
    z80_registers[real_index].word = value;
  }
  return 0;
}

int register_value_set(cpu_t *cpu, uint8_t index, uint16_t value) {
  uint8_t real_index = index & REG_MASK;

  if (!cpu)
    return -1;

  if (real_index >= REG_COUNT)
    return -1;

  if (index & HIGH_BYTE) {
    cpu->registers[real_index].bytes.high = value;
  } else if (index & LOW_BYTE) {
    cpu->registers[real_index].bytes.low = value;
  } else {
    cpu->registers[real_index].word = value;
  }
  return 0;
}

uint16_t _register_value_get(z80_register_t *z80_registers, uint8_t index) {
  uint8_t real_index = index & REG_MASK;
  uint16_t return_value = 0;

  if (real_index >= REG_COUNT)
    return return_value;

  if (index & HIGH_BYTE) {
    return_value = z80_registers[real_index].bytes.high;
  } else if (index & LOW_BYTE) {
    return_value = z80_registers[real_index].bytes.low;
  } else {
    return_value = z80_registers[real_index].word;
  }
  return return_value;
}

uint16_t register_value_get(cpu_t *cpu, uint8_t index) {
  uint8_t real_index = index & REG_MASK;
  uint16_t return_value = 0;

  if (!cpu)
    return return_value;

  if (real_index >= REG_COUNT)
    return return_value;

  if (index & HIGH_BYTE) {
    return_value = cpu->registers[real_index].bytes.high;
  } else if (index & LOW_BYTE) {
    return_value = cpu->registers[real_index].bytes.low;
  } else {
    return_value = cpu->registers[real_index].word;
  }
  return return_value;
}

void register_inc(cpu_t *cpu, uint8_t index) {
  uint8_t real_index = index & REG_MASK;

  if (!cpu)
    return;

  if (real_index >= REG_COUNT)
    return;

  if (index & HIGH_BYTE) {
    cpu->registers[real_index].bytes.high++;
  } else if (index & LOW_BYTE) {
    cpu->registers[real_index].bytes.low++;
  } else {
    cpu->registers[real_index].word++;
  }
  return;
}

void register_dec(cpu_t *cpu, uint8_t index) {
  uint8_t real_index = index & REG_MASK;

  if (!cpu)
    return;

  if (real_index >= REG_COUNT)
    return;

  if (index & HIGH_BYTE) {
    cpu->registers[real_index].bytes.high--;
  } else if (index & LOW_BYTE) {
    cpu->registers[real_index].bytes.low--;
  } else {
    cpu->registers[real_index].word--;
  }
  return;
}

static void _flag_display(uint16_t flags) {
  fprintf(stdout, "S:%1d Z:%1d H:%1d PV:%1d N:%1d C:%1d",
          flags & (1 << FLAG_S) ? 1 : 0, flags & (1 << FLAG_Z) ? 1 : 0,
          flags & (1 << FLAG_H) ? 1 : 0, flags & (1 << FLAG_PV) ? 1 : 0,
          flags & (1 << FLAG_N) ? 1 : 0, flags & (1 << FLAG_C) ? 1 : 0);
}

static void _dump_memory_window(cpu_t *cpu, uint16_t address) {
  uint16_t base = (uint16_t)(address & 0xFFF0);

  for (uint16_t row = 0; row < 2; row++) {
    uint16_t row_addr = (uint16_t)(base + row * 16);
    fprintf(stdout, "  %04X  ", row_addr);
    for (uint16_t col = 0; col < 16; col++) {
      uint16_t addr = (uint16_t)(row_addr + col);
      fprintf(stdout, "%02X ", (uint8_t)cpu->memory.memory[addr]);
    }
    fprintf(stdout, " |");
    for (uint16_t col = 0; col < 16; col++) {
      uint16_t addr = (uint16_t)(row_addr + col);
      uint8_t value = (uint8_t)cpu->memory.memory[addr];
      fputc(isprint(value) ? value : '.', stdout);
    }
    fprintf(stdout, "|\n");
  }
}

void register_swap_single(cpu_t *cpu, uint8_t reg) {
  z80_register_t temp;

  if (!cpu)
    return;

  temp = cpu->registers[reg];
  cpu->registers[reg] = cpu->alt_registers[reg];
  cpu->alt_registers[reg] = temp;
}

void register_exx(cpu_t *cpu) {
  if (!cpu)
    return;

  register_swap_single(cpu, REG_BC);
  register_swap_single(cpu, REG_DE);
  register_swap_single(cpu, REG_HL);
}

void register_ex_de_hl(cpu_t *cpu) {
  z80_register_t temp;

  if (!cpu)
    return;

  temp = cpu->registers[REG_DE];
  cpu->registers[REG_DE] = cpu->registers[REG_HL];
  cpu->registers[REG_HL] = temp;
}

void register_ex_af_af_alt(cpu_t *cpu) {
  z80_register_t temp;

  if (!cpu)
    return;

  temp = cpu->registers[REG_AF];
  cpu->registers[REG_AF] = cpu->alt_registers[REG_AF];
  cpu->alt_registers[REG_AF] = temp;
}

static void register_ex_sp_rr(cpu_t *cpu, uint8_t reg) {
  uint16_t sp = register_value_get(cpu, REG_SP);
  uint8_t low = memory_get(cpu, sp);
  uint8_t high = memory_get(cpu, (uint16_t)(sp + 1));
  uint16_t mem_value = (uint16_t)((high << 8) | low);
  uint16_t reg_value = register_value_get(cpu, reg);

  register_value_set(cpu, reg, mem_value);
  memory_set(cpu, sp, (uint8_t)(reg_value & 0x00FF));
  memory_set(cpu, (uint16_t)(sp + 1), (uint8_t)((reg_value >> 8) & 0x00FF));
}

void register_ex_sp_hl(cpu_t *cpu) { register_ex_sp_rr(cpu, REG_HL); }

void register_ex_sp_ix(cpu_t *cpu) { register_ex_sp_rr(cpu, REG_IX); }

void register_ex_sp_iy(cpu_t *cpu) { register_ex_sp_rr(cpu, REG_IY); }

void register_display(cpu_t *cpu) {
  if (!cpu)
    return;

  fprintf(stdout, "\033[2J\033[H");
  fprintf(stdout, "\033[1;36mZ80 CPU State\033[0m\n");
  fprintf(stdout, "AF:%04X  BC:%04X  DE:%04X  HL:%04X  IX:%04X  IY:%04X\n",
          _register_value_get(cpu->registers, REG_AF),
          _register_value_get(cpu->registers, REG_BC),
          _register_value_get(cpu->registers, REG_DE),
          _register_value_get(cpu->registers, REG_HL),
          _register_value_get(cpu->registers, REG_IX),
          _register_value_get(cpu->registers, REG_IY));
  fprintf(stdout, "SP:%04X  PC:%04X  IR:%04X  ",
          _register_value_get(cpu->registers, REG_SP),
          _register_value_get(cpu->registers, REG_PC),
          _register_value_get(cpu->registers, REG_IR));
  _flag_display(_register_value_get(cpu->registers, REG_F) & 0xFF);
  fprintf(stdout, "\n\n");

  fprintf(stdout, "\033[1;34mAlt Registers\033[0m\n");
  fprintf(stdout, "AF':%04X  BC':%04X  DE':%04X  HL':%04X\n\n",
          _register_value_get(cpu->alt_registers, REG_AF),
          _register_value_get(cpu->alt_registers, REG_BC),
          _register_value_get(cpu->alt_registers, REG_DE),
          _register_value_get(cpu->alt_registers, REG_HL));

  fprintf(stdout, "\033[1;33mInstruction\033[0m\n");
  if (cpu->last_instruction[0] != '\0') {
    fprintf(stdout, "  %s\n\n", cpu->last_instruction);
  } else {
    fprintf(stdout, "  (none)\n\n");
  }

  fprintf(stdout, "\033[1;35mMemory (Last Read)\033[0m\n");
  if (cpu->last_mem_read_valid) {
    fprintf(stdout, "  Address: %04X\n", cpu->last_mem_read);
    _dump_memory_window(cpu, cpu->last_mem_read);
  } else {
    fprintf(stdout, "  (none)\n");
  }
  fprintf(stdout, "\n");

  fprintf(stdout, "\033[1;35mMemory (Last Write)\033[0m\n");
  if (cpu->last_mem_write_valid) {
    fprintf(stdout, "  Address: %04X\n", cpu->last_mem_write);
    _dump_memory_window(cpu, cpu->last_mem_write);
  } else {
    fprintf(stdout, "  (none)\n");
  }
}

void register_bit_set(cpu_t *cpu, uint8_t index, uint8_t bit) {
  uint8_t current_value = register_value_get(cpu, index) & 0x00FF;
  uint8_t new_value = current_value | (1 << bit);
  register_value_set(cpu, index, new_value);
}
void register_bit_unset(cpu_t *cpu, uint8_t index, uint8_t bit) {
  uint8_t current_value = register_value_get(cpu, REG_F) & 0x00FF;
  uint8_t new_value = current_value & ~(1 << bit);
  register_value_set(cpu, index, new_value);
}

uint8_t register_bit_get(cpu_t *cpu, uint8_t index, uint8_t bit) {
  uint8_t current_value = register_value_get(cpu, REG_F) & 0x00FF;
  return ((current_value & (1 << bit)) > 0);
}

void register_flag_set(cpu_t *cpu, uint8_t flag) {
  register_bit_set(cpu, REG_F, flag);
}

void register_flag_unset(cpu_t *cpu, uint8_t flag) {
  register_bit_set(cpu, REG_F, flag);
}

uint8_t register_map(uint8_t index) {
  if (index >= sizeof(_reg_map))
    return 0xFF;
  return _reg_map[index];
}
