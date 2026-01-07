#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    I_NOP,
    I_LOAD_R_R,
    I_LOAD_R_N,
    I_LOAD_R_HL,
    I_LOAD_R_IDX,
    I_LOAD_HL_R,
    I_LOAD_IDX_R,
    I_LOAD_A_MEM,
    I_LOAD_A_RR,
    I_LOAD_MEM_A,
    I_LOAD_RR_A,
    I_LOAD_A_I,
    I_LOAD_A_R_REG,
    I_LOAD_I_A,
    I_LOAD_R_REG_A,
    I_U // Undefined or unused
} instruction_group_t;

typedef struct {
uint16_t op_code;
instruction_group_t group;
} instruction_map_t;

instruction_group_t instruction_group_get(uint16_t);

void _load_r_r(uint8_t, uint8_t);
void inst_load_r_r(uint8_t op_code);
void inst_load_r_n(uint8_t op_code, uint8_t value);
void inst_load_r_hl(uint8_t op_code);
void inst_load_r_idx(uint8_t op_code, uint8_t index_reg, uint8_t d);
void inst_load_hl_r(uint8_t op_code);
void inst_load_idx_r(uint8_t op_code, uint8_t index_reg, uint8_t d);
void inst_load_hl_n(uint8_t value);
void inst_load_idx_n(uint8_t index_reg, uint8_t d, uint8_t value);
void inst_load_a_mem(uint16_t address);
void inst_load_a_rr(uint8_t rr);
void inst_load_mem_a(uint16_t address);
void inst_load_rr_a(uint8_t rr);