#ifndef REGISTER_H
#define REGISTER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "cpu_fwd.h"

typedef union {
  uint16_t word;
  struct {
    uint8_t low;
    uint8_t high;
  } bytes;
} z80_register_t;

#define HIGH_BYTE 0x80
#define LOW_BYTE 0x40
#define REG_AF 0
#define REG_A (HIGH_BYTE | REG_AF)
#define REG_F (LOW_BYTE | REG_AF)
#define REG_BC 1
#define REG_B (HIGH_BYTE | REG_BC)
#define REG_C (LOW_BYTE | REG_BC)
#define REG_DE 2
#define REG_D (HIGH_BYTE | REG_DE)
#define REG_E (LOW_BYTE | REG_DE)
#define REG_HL 3
#define REG_H (HIGH_BYTE | REG_HL)
#define REG_L (LOW_BYTE | REG_HL)
#define REG_IR 4
#define REG_I (HIGH_BYTE | REG_IR)
#define REG_R (LOW_BYTE | REG_IR)
#define REG_IX 5
#define REG_IY 6
#define REG_SP 7
#define REG_PC 8

#define REG_COUNT 9
#define REG_MASK 0x0F

#define FLAG_S 7
#define FLAG_Z 6
#define FLAG_H 4
#define FLAG_PV 2
#define FLAG_N 1
#define FLAG_C 0

#define FLAG_IS_NEGATIVE(f) ((f & 0x00FF) & (1<<FLAG_N))
#define FLAG_IS_ZERO(f) ((f & 0x00FF) & (1<<FLAG_Z))
#define FLAG_IS_HALF(f) ((f & 0x00FF) & (1<<FLAG_H))
#define FLAG_IS_OVERFLOW(f) ((f & 0x00FF) & (1<<FLAG_PV))
#define FLAG_IS_EVEN(f) ((f & 0x00FF) & (1<<FLAG_PV))
#define FLAG_IS_SUBTRACT(f) ((f & 0x00FF) & (1<<FLAG_S))
#define FLAG_IS_CARRY(f) ((f & 0x00FF) & (1<<FLAG_C))

#define REG_TRUE (1 == 1)
#define REG_FALSE (1 == 0)

int register_init(cpu_t *cpu);
int register_destroy(cpu_t *cpu);

void register_display(cpu_t *cpu);

int register_value_set(cpu_t *cpu, uint8_t, uint16_t);
uint16_t register_value_get(cpu_t *cpu, uint8_t);

void register_inc(cpu_t *cpu, uint8_t);
void register_dec(cpu_t *cpu, uint8_t);

void register_flag_set(cpu_t *cpu, uint8_t);
void register_flag_unset(cpu_t *cpu, uint8_t);

void register_bit_set(cpu_t *cpu, uint8_t, uint8_t);
void register_bit_unset(cpu_t *cpu, uint8_t, uint8_t);
uint8_t register_bit_get(cpu_t *cpu, uint8_t, uint8_t);

uint8_t register_map(uint8_t index);

void register_swap_single(cpu_t *cpu, uint8_t);
void register_exx(cpu_t *cpu);
void register_ex_de_hl(cpu_t *cpu);
void register_ex_af_af_alt(cpu_t *cpu);
void register_ex_sp_hl(cpu_t *cpu);
void register_ex_sp_ix(cpu_t *cpu);
void register_ex_sp_iy(cpu_t *cpu);

#endif
