#ifndef REGISTER_H
#define REGISTER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef union {
    uint16_t af;
    struct {
        uint8_t f;
        uint8_t a;
    };
} reg_af_t;

typedef union {
    uint16_t bc;
    struct {
        uint8_t c;
        uint8_t b;
    };
} reg_bc_t;

typedef union {
    uint16_t de;
    struct {
        uint8_t e;
        uint8_t d;
    };
} reg_de_t;

typedef union {
    uint16_t hl;
    struct {
        uint8_t l;
        uint8_t h;
    };
} reg_hl_t;

typedef struct {
    reg_af_t reg_af;
    reg_bc_t reg_bc;
    reg_de_t reg_de;
    reg_hl_t reg_hl;
} z80_general_register_t;

typedef struct {
    uint8_t     i;
    uint8_t     r;
    uint16_t    ix;
    uint16_t    iy;
    uint16_t    sp;
    uint16_t    pc;
} z80_special_register_t;

#endif
