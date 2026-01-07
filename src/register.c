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
  fprintf(stdout, "\tS:%1d Z:%1d H:%1d PV:%1d N:%1d C:%1d\n",
          flags & (1 << FLAG_S) ? 1 : 0, flags & (1 << FLAG_Z) ? 1 : 0,
          flags & (1 << FLAG_H) ? 1 : 0, flags & (1 << FLAG_PV) ? 1 : 0,
          flags & (1 << FLAG_N) ? 1 : 0, flags & (1 << FLAG_C) ? 1 : 0);

  if (FLAG_IS_CARRY(flags))
    fprintf(stdout, "\tCarry");
  if (FLAG_IS_ZERO(flags))
    fprintf(stdout, "\tZero");
  fprintf(stdout, "\t%s", FLAG_IS_EVEN(flags) ? "Even" : "Odd");
  if (FLAG_IS_OVERFLOW(flags))
    fprintf(stdout, "\tOverflow");
  if (FLAG_IS_HALF(flags))
    fprintf(stdout, "\tHalf-Carry");
  if (FLAG_IS_NEGATIVE(flags))
    fprintf(stdout, "\tNegative");

  fprintf(stdout, "\n");
}

static void _register_display(z80_register_t *z80_registers, int alt) {
  fprintf(stdout, "=========\n");
  fprintf(stdout, "%s: %04X\t%s: %02X\t%s: %02X\n", alt ? "AF'" : " AF",
          _register_value_get(z80_registers, REG_AF), alt ? "A'" : " A",
          _register_value_get(z80_registers, REG_A), alt ? "F'" : " F",
          _register_value_get(z80_registers, REG_F));
  _flag_display(_register_value_get(z80_registers, REG_F) & 0xFF);
  fprintf(stdout, "%s: %04X\t%s: %02X\t%s: %02X\n", alt ? "BC'" : " BC",
          _register_value_get(z80_registers, REG_BC), alt ? "B'" : " B",
          _register_value_get(z80_registers, REG_D), alt ? "C'" : " C",
          _register_value_get(z80_registers, REG_E));
  fprintf(stdout, "%s: %04X\t%s: %02X\t%s: %02X\n", alt ? "HL'" : " HL",
          _register_value_get(z80_registers, REG_HL), alt ? "H'" : " H",
          _register_value_get(z80_registers, REG_H), alt ? "L'" : " L",
          _register_value_get(z80_registers, REG_L));
  fprintf(stdout, "%s: %04X", alt ? "PC'" : " PC",
          _register_value_get(z80_registers, REG_PC));
  fprintf(stdout, "\t%s: %04X\n", alt ? "SP'" : " SP",
          _register_value_get(z80_registers, REG_SP));
  fprintf(stdout, "%s: %04X", alt ? "IX'" : " IX",
          _register_value_get(z80_registers, REG_IX));
  fprintf(stdout, "\t%s: %04X\n", alt ? "IY'" : " IY",
          _register_value_get(z80_registers, REG_IY));
  fprintf(stdout, "%s: %04X\t%s: %02X\t%s: %02X\n", alt ? "IR'" : " IR",
          _register_value_get(z80_registers, REG_IR), alt ? "I'" : " I",
          _register_value_get(z80_registers, REG_I), alt ? "R'" : " R",
          _register_value_get(z80_registers, REG_R));
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

  _register_display(cpu->registers, REG_FALSE);
  _register_display(cpu->alt_registers, REG_TRUE);
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
