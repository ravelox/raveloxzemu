#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu_fwd.h"

typedef enum {
    I_NOP,
    I_LOAD_R_R,
    I_LOAD_R_N,
    I_LOAD_R_HL,
    I_LOAD_R_IDX,
    I_LOAD_HL_R,
    I_LOAD_HL_N,
    I_LOAD_IDX_R,
    I_LOAD_IDX_N,
    I_LOAD_A_MEM,
    I_LOAD_A_RR,
    I_LOAD_MEM_A,
    I_LOAD_RR_A,
    I_LOAD_A_I,
    I_LOAD_A_R_REG,
    I_LOAD_I_A,
    I_LOAD_R_REG_A,
    I_LOAD_RR_NN,
    I_LOAD_RR_MEM,
    I_LOAD_MEM_RR,
    I_LOAD_SP_RR,
    I_LOAD_SP_MEM,
    I_LOAD_MEM_SP,
    I_PUSH,
    I_POP,
    I_BLKT,
    I_BLKS,
    I_EX,
    I_U // Undefined or unused
} instruction_group_t;

typedef struct {
uint16_t op_code;
instruction_group_t group;
const char *label;
} instruction_map_t;

instruction_group_t instruction_group_get(uint16_t);

void _load_r_r(cpu_t *cpu, uint8_t, uint8_t);
void _load_r_from_mem(cpu_t *cpu, uint8_t reg, uint16_t address);
void _load_mem_from_mem(cpu_t *cpu, uint16_t source, uint16_t dest);
uint16_t _load_word_from_mem(cpu_t *cpu, uint16_t address);
void _store_word_to_mem(cpu_t *cpu, uint16_t address, uint16_t value);
void inst_load_r_r(cpu_t *cpu, uint8_t op_code);
void inst_load_r_n(cpu_t *cpu, uint8_t op_code, uint8_t value);
void inst_load_r_hl(cpu_t *cpu, uint8_t op_code);
void inst_load_r_idx(cpu_t *cpu, uint8_t op_code, uint8_t index_reg,
                     uint8_t d);
void inst_load_hl_r(cpu_t *cpu, uint8_t op_code);
void inst_load_idx_r(cpu_t *cpu, uint8_t op_code, uint8_t index_reg,
                     uint8_t d);
void inst_load_hl_n(cpu_t *cpu, uint8_t value);
void inst_load_idx_n(cpu_t *cpu, uint8_t index_reg, uint8_t d, uint8_t value);
void inst_load_a_mem(cpu_t *cpu, uint16_t address);
void inst_load_a_rr(cpu_t *cpu, uint8_t rr);
void inst_load_mem_a(cpu_t *cpu, uint16_t address);
void inst_load_rr_a(cpu_t *cpu, uint8_t rr);
void inst_load_rr_nn(cpu_t *cpu, uint8_t rr, uint16_t value);
void inst_load_rr_mem(cpu_t *cpu, uint8_t rr, uint16_t address);
void inst_load_mem_rr(cpu_t *cpu, uint8_t rr, uint16_t address);
void inst_load_sp_rr(cpu_t *cpu, uint8_t rr);
void inst_load_sp_mem(cpu_t *cpu, uint16_t address);
void inst_load_mem_sp(cpu_t *cpu, uint16_t address);
void inst_push_rr(cpu_t *cpu, uint8_t rr);
void inst_pop_rr(cpu_t *cpu, uint8_t rr);

void inst_blkt(cpu_t *cpu, uint16_t);
void inst_blks(cpu_t *cpu, uint16_t);
