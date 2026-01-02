#ifndef REGISTER_H
#define REGISTER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef union {
  uint16_t af;
  struct {
    uint8_t a;
    union {
      uint8_t f;
      struct {
        uint8_t s : 1;
        uint8_t z : 1;
        uint8_t unused1 : 1;
        uint8_t h : 1;
        uint8_t unused2 : 1;
        uint8_t pv : 1;
        uint8_t n : 1;
        uint8_t c : 1;
      } flags;
    } reg_f;
  };
} reg_af_t;

typedef union {
  uint16_t bc;
  struct {
    uint8_t b;
    uint8_t c;
  };
} reg_bc_t;

typedef union {
  uint16_t de;
  struct {
    uint8_t d;
    uint8_t e;
  };
} reg_de_t;

typedef union {
  uint16_t hl;
  struct {
    uint8_t h;
    uint8_t l;
  };
} reg_hl_t;

typedef struct {
  reg_af_t reg_af;
  reg_bc_t reg_bc;
  reg_de_t reg_de;
  reg_hl_t reg_hl;
} z80_general_register_t;

typedef struct {
  uint8_t i;
  uint8_t r;
  uint16_t ix;
  uint16_t iy;
  uint16_t sp;
  uint16_t pc;
} z80_special_register_t;

int register_init(void);
void register_display(void);
int register_destroy(void);

void register_set_a(uint8_t value);
void register_set_f(uint8_t value);
void register_set_b(uint8_t value);
void register_set_c(uint8_t value);
void register_set_d(uint8_t value);
void register_set_e(uint8_t value);
void register_set_h(uint8_t value);
void register_set_l(uint8_t value);

void register_set_af(uint16_t value);
void register_set_bc(uint16_t value);
void register_set_de(uint16_t value);
void register_set_hl(uint16_t value);

void register_set_ix(uint16_t value);
void register_set_iy(uint16_t value);
void register_set_pc(uint16_t value);
void register_set_sp(uint16_t value);

void register_general_swap(void);
void register_special_swap(void);

#endif
