#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "register.h"

static z80_general_register_t *general;
static z80_general_register_t *alt_general;

static z80_special_register_t *special;
static z80_special_register_t *alt_special;

int register_init(void) {

  general = (z80_general_register_t *)malloc(sizeof(z80_general_register_t));
  alt_general =
      (z80_general_register_t *)malloc(sizeof(z80_general_register_t));
  special = (z80_special_register_t *)malloc(sizeof(z80_special_register_t));
  alt_special =
      (z80_special_register_t *)malloc(sizeof(z80_special_register_t));

  if (!general || !alt_general || !special || !alt_special) {
    fprintf(stderr, "Cannot allocate memory for registers\n");
    return -1;
  }

  memset(general, 0, sizeof(z80_general_register_t));
  memset(alt_general, 0, sizeof(z80_general_register_t));
  memset(special, 0, sizeof(z80_special_register_t));
  memset(alt_special, 0, sizeof(z80_special_register_t));

  return 0;
}

int register_destroy(void) {
  if (general)
    free(general);
  if (alt_general)
    free(alt_general);
  if (special)
    free(special);
  if (alt_special)
    free(alt_special);

  return 0;
}

void register_general_swap(void) {
  z80_general_register_t *temp;

  temp = general;
  general = alt_general;
  alt_general = temp;
}

void register_special_swap(void) {
  z80_special_register_t *temp;

  temp = special;
  special = alt_special;
  alt_special = temp;
}

static void register_special_display(z80_special_register_t *reg) {
  fprintf(stdout, "PC: %04d\tSP: %04d\n", reg->pc, reg->sp);
  fprintf(stdout, "IX: %04d\tIY: %04d\n", reg->ix, reg->iy);
  fprintf(stdout, " I: %02d  \t R: %02d\n", reg->i, reg->r);
}

static void register_general_display(z80_general_register_t *reg) {
  fprintf(stdout, "AF: %04d\tA: %02d\tF: %02d\n", reg->reg_af.af, reg->reg_af.a,
          reg->reg_af.reg_f.f);
  fprintf(stdout, "Flags:");
  fprintf(stdout, "\t S:%d", reg->reg_af.reg_f.flags.s);
  fprintf(stdout, "\t Z:%d", reg->reg_af.reg_f.flags.z);
  fprintf(stdout, "\t H:%d", reg->reg_af.reg_f.flags.h);
  fprintf(stdout, "\tPV:%d", reg->reg_af.reg_f.flags.pv);
  fprintf(stdout, "\t N:%d", reg->reg_af.reg_f.flags.n);
  fprintf(stdout, "\t C:%d", reg->reg_af.reg_f.flags.c);
  fprintf(stdout, "\n");
  fprintf(stdout, "BC: %04d\tB: %02d\tC: %02d\n", reg->reg_bc.bc, reg->reg_bc.b,
          reg->reg_bc.c);
  fprintf(stdout, "DE: %04d\tD: %02d\tE: %02d\n", reg->reg_de.de, reg->reg_de.d,
          reg->reg_de.e);
  fprintf(stdout, "HL: %04d\tH: %02d\tL: %02d\n", reg->reg_hl.hl, reg->reg_hl.h,
          reg->reg_hl.l);
}

void register_display(void) {
  register_special_display(special);
  fprintf(stdout, "\nALT:\n");
  register_special_display(alt_special);
  fprintf(stdout, "\n\n");
  register_general_display(general);
  fprintf(stdout, "\nALT\n");
  register_general_display(alt_general);
}

// 8-bit registers
void register_set_a(uint8_t value) { general->reg_af.a = value; }
void register_set_f(uint8_t value) { general->reg_af.reg_f.f = value; }
void register_set_b(uint8_t value) { general->reg_bc.b = value; }
void register_set_c(uint8_t value) { general->reg_bc.c = value; }
void register_set_d(uint8_t value) { general->reg_de.d = value; }
void register_set_e(uint8_t value) { general->reg_de.e = value; }
void register_set_h(uint8_t value) { general->reg_hl.h = value; }
void register_set_l(uint8_t value) { general->reg_hl.l = value; }

// 16-bit registers
void register_set_af(uint16_t value) { general->reg_af.af = value; }
void register_set_bc(uint16_t value) { general->reg_bc.bc = value; }
void register_set_de(uint16_t value) { general->reg_de.de = value; }
void register_set_hl(uint16_t value) { general->reg_hl.hl = value; }
void register_set_pc(uint16_t value) { special->pc = value; }
void register_set_sp(uint16_t value) { special->sp = value; }
void register_set_ix(uint16_t value) { special->ix = value; }
void register_set_iy(uint16_t value) { special->iy = value; }
