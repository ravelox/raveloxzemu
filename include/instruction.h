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
    I_ADD_A_R,
    I_ADD_A_N,
    I_ADD_A_IDX,
    I_ADC_A_R,
    I_ADC_A_N,
    I_ADC_A_IDX,
    I_SUB_R,
    I_SUB_N,
    I_SUB_IDX,
    I_SBC_A_R,
    I_SBC_A_N,
    I_SBC_A_IDX,
    I_INC_R,
    I_INC_HL,
    I_INC_IDX,
    I_DEC_R,
    I_DEC_HL,
    I_DEC_IDX,
    I_AND_R,
    I_AND_N,
    I_AND_IDX,
    I_OR_R,
    I_OR_N,
    I_OR_IDX,
    I_XOR_R,
    I_XOR_N,
    I_XOR_IDX,
    I_CP_R,
    I_CP_N,
    I_CP_IDX,
    I_JR,
    I_JP,
    I_CALL,
    I_RET,
    I_RST,
    I_ADD_HL_RR,
    I_ADD_IX_IY_RR,
    I_ADC_HL_RR,
    I_SBC_HL_RR,
    I_INC_RR,
    I_DEC_RR,
    I_DAA,
    I_CPL,
    I_NEG,
    I_CCF,
    I_SCF,
    I_HALT,
    I_DI,
    I_EI,
    I_IM,
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
void instruction_map_init(void);

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
void inst_add_a_r(cpu_t *cpu, uint8_t op_code);
void inst_add_a_n(cpu_t *cpu, uint8_t value);
void inst_add_a_idx(cpu_t *cpu, uint8_t index_reg, uint8_t d);
void inst_adc_a_r(cpu_t *cpu, uint8_t op_code);
void inst_adc_a_n(cpu_t *cpu, uint8_t value);
void inst_adc_a_idx(cpu_t *cpu, uint8_t index_reg, uint8_t d);
void inst_sub_r(cpu_t *cpu, uint8_t op_code);
void inst_sub_n(cpu_t *cpu, uint8_t value);
void inst_sub_idx(cpu_t *cpu, uint8_t index_reg, uint8_t d);
void inst_sbc_a_r(cpu_t *cpu, uint8_t op_code);
void inst_sbc_a_n(cpu_t *cpu, uint8_t value);
void inst_sbc_a_idx(cpu_t *cpu, uint8_t index_reg, uint8_t d);
void inst_inc_r(cpu_t *cpu, uint8_t op_code);
void inst_inc_hl(cpu_t *cpu);
void inst_inc_idx(cpu_t *cpu, uint8_t index_reg, uint8_t d);
void inst_dec_r(cpu_t *cpu, uint8_t op_code);
void inst_dec_hl(cpu_t *cpu);
void inst_dec_idx(cpu_t *cpu, uint8_t index_reg, uint8_t d);
void inst_and_r(cpu_t *cpu, uint8_t op_code);
void inst_and_n(cpu_t *cpu, uint8_t value);
void inst_and_idx(cpu_t *cpu, uint8_t index_reg, uint8_t d);
void inst_or_r(cpu_t *cpu, uint8_t op_code);
void inst_or_n(cpu_t *cpu, uint8_t value);
void inst_or_idx(cpu_t *cpu, uint8_t index_reg, uint8_t d);
void inst_xor_r(cpu_t *cpu, uint8_t op_code);
void inst_xor_n(cpu_t *cpu, uint8_t value);
void inst_xor_idx(cpu_t *cpu, uint8_t index_reg, uint8_t d);
void inst_cp_r(cpu_t *cpu, uint8_t op_code);
void inst_cp_n(cpu_t *cpu, uint8_t value);
void inst_cp_idx(cpu_t *cpu, uint8_t index_reg, uint8_t d);
void inst_jr(cpu_t *cpu, uint8_t op_code, uint8_t displacement);
void inst_jp(cpu_t *cpu, uint16_t op_code, uint16_t address);
void inst_call(cpu_t *cpu, uint16_t op_code, uint16_t address);
void inst_ret(cpu_t *cpu, uint16_t op_code);
void inst_rst(cpu_t *cpu, uint8_t op_code);
void inst_add_hl_rr(cpu_t *cpu, uint16_t op_code);
void inst_add_ix_iy_rr(cpu_t *cpu, uint16_t op_code, uint8_t index_reg);
void inst_adc_hl_rr(cpu_t *cpu, uint16_t op_code);
void inst_sbc_hl_rr(cpu_t *cpu, uint16_t op_code);
void inst_inc_rr(cpu_t *cpu, uint16_t op_code);
void inst_dec_rr(cpu_t *cpu, uint16_t op_code);
void inst_daa(cpu_t *cpu);
void inst_cpl(cpu_t *cpu);
void inst_neg(cpu_t *cpu);
void inst_ccf(cpu_t *cpu);
void inst_scf(cpu_t *cpu);
void inst_halt(cpu_t *cpu);
void inst_di(cpu_t *cpu);
void inst_ei(cpu_t *cpu);
void inst_im(cpu_t *cpu, uint8_t mode);
void inst_cb(cpu_t *cpu, uint8_t op_code, uint8_t use_index, uint8_t index_reg,
             uint8_t d);

void inst_blkt(cpu_t *cpu, uint16_t);
void inst_blks(cpu_t *cpu, uint16_t);
