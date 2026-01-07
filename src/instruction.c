#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "instruction.h"
#include "memory.h"
#include "register.h"

static instruction_map_t instruction_map[] = {
{0x00,I_NOP},
{0x02, I_LOAD_RR_A},
{0x06,I_LOAD_R_N},
{0x0A,I_LOAD_RR_A},
{0x0E,I_LOAD_R_N},
{0x12, I_LOAD_RR_A},
{0x16,I_LOAD_R_N},
{0x1A,I_LOAD_RR_A},
{0x1E,I_LOAD_R_N},
{0x26,I_LOAD_R_N},
{0x2E,I_LOAD_R_N},
{0x32, I_LOAD_MEM_A},
{0x3A,I_LOAD_A_MEM},
{0x3E,I_LOAD_R_N},
{0x36,I_LOAD_HL_N},
{0x40,I_LOAD_R_R},
{0x41,I_LOAD_R_R},
{0x42,I_LOAD_R_R},
{0x43,I_LOAD_R_R},
{0x44,I_LOAD_R_R},
{0x45,I_LOAD_R_R},
{0x46,I_LOAD_HL_R},
{0x47,I_LOAD_R_R},
{0x48,I_LOAD_R_R},
{0x49,I_LOAD_R_R},
{0x4A,I_LOAD_R_R},
{0x4B,I_LOAD_R_R},
{0x4C,I_LOAD_R_R},
{0x4D,I_LOAD_R_R},
{0x4E,I_LOAD_HL_R},
{0x4F,I_LOAD_R_R},
{0x50,I_LOAD_R_R},
{0x51,I_LOAD_R_R},
{0x52,I_LOAD_R_R},
{0x53,I_LOAD_R_R},
{0x54,I_LOAD_R_R},
{0x55,I_LOAD_R_R},
{0x56,I_LOAD_HL_R},
{0x57,I_LOAD_R_R},
{0x58,I_LOAD_R_R},
{0x59,I_LOAD_R_R},
{0x5A,I_LOAD_R_R},
{0x5B,I_LOAD_R_R},
{0x5C,I_LOAD_R_R},
{0x5D,I_LOAD_R_R},
{0x5E,I_LOAD_HL_R},
{0x5F,I_LOAD_R_R},
{0x60,I_LOAD_R_R},
{0x61,I_LOAD_R_R},
{0x62,I_LOAD_R_R},
{0x63,I_LOAD_R_R},
{0x64,I_LOAD_R_R},
{0x65,I_LOAD_R_R},
{0x66,I_LOAD_HL_R},
{0x67,I_LOAD_R_R},
{0x68,I_LOAD_R_R},
{0x69,I_LOAD_R_R},
{0x6A,I_LOAD_R_R},
{0x6B,I_LOAD_R_R},
{0x6C,I_LOAD_R_R},
{0x6D,I_LOAD_R_R},
{0x6E,I_LOAD_HL_R},
{0x6F,I_LOAD_R_R},
{0x7E,I_LOAD_HL_R},
{0x70,I_LOAD_HL_R},
{0x71,I_LOAD_HL_R},
{0x72,I_LOAD_HL_R},
{0x73,I_LOAD_HL_R},
{0x74,I_LOAD_HL_R},
{0x75,I_LOAD_HL_R},
{0x77,I_LOAD_HL_R},
{0x78,I_LOAD_R_R},
{0x79,I_LOAD_R_R},
{0x7A,I_LOAD_R_R},
{0x7B,I_LOAD_R_R},
{0x7C,I_LOAD_R_R},
{0x7D,I_LOAD_R_R},
{0x7F,I_LOAD_R_R},
{0xDD36, I_LOAD_IDX_N},
{0xDD46,I_LOAD_R_IDX},
{0xDD4E,I_LOAD_R_IDX},
{0xDD56,I_LOAD_R_IDX},
{0xDD5E,I_LOAD_R_IDX},
{0xDD66,I_LOAD_R_IDX},
{0xDD6E,I_LOAD_R_IDX},
{0xDD70,I_LOAD_IDX_R},
{0xDD71,I_LOAD_IDX_R},
{0xDD72,I_LOAD_IDX_R},
{0xDD73,I_LOAD_IDX_R},
{0xDD74,I_LOAD_IDX_R},
{0xDD75,I_LOAD_IDX_R},
{0xDD77,I_LOAD_IDX_R},
{0xDD7E,I_LOAD_IDX_R},
{0xDD7E,I_LOAD_R_IDX},
{0xED47, I_LOAD_A_I},
{0xED4F, I_LOAD_A_R_REG},
{0xED57, I_LOAD_I_A},
{0xED5F, I_LOAD_R_REG_A},
{0XFD36, I_LOAD_IDX_N},
{0xFD46,I_LOAD_R_IDX},
{0xFD4E,I_LOAD_R_IDX},
{0xFD56,I_LOAD_R_IDX},
{0xFD5E,I_LOAD_R_IDX},
{0xFD66,I_LOAD_R_IDX},
{0xFD6E,I_LOAD_R_IDX},
{0xFD70,I_LOAD_IDX_R},
{0xFD71,I_LOAD_IDX_R},
{0xFD72,I_LOAD_IDX_R},
{0xFD73,I_LOAD_IDX_R},
{0xFD74,I_LOAD_IDX_R},
{0xFD75,I_LOAD_IDX_R},
{0xFD77,I_LOAD_IDX_R},
{0xFD7E,I_LOAD_IDX_R},
{0xFD7E,I_LOAD_R_IDX},
{0xFF,I_U}};

instruction_group_t instruction_group_get(uint16_t op_code) {
  for (uint16_t index = 0; index < sizeof(instruction_map); index++) {
    if (instruction_map[index].op_code == op_code)
      return instruction_map[index].group;
  }

  return I_U;
}

void _load_r_r(uint8_t source, uint8_t dest) {
  uint8_t value = register_value_get(source);
  register_value_set(dest, value);
}

void _load_r_from_mem(uint8_t reg, uint16_t address) {
  uint8_t value = memory_get(address);
  register_value_set(reg, value);
}

void inst_load_r_r(uint8_t op_code) {
  uint8_t source, dest;
  dest = register_map((op_code & 0x38) >> 3);
  source = register_map(op_code & 0x7);
  _load_r_r(source, dest);
}

void inst_load_r_n(uint8_t op_code, uint8_t value) {
  uint8_t dest;
  dest = register_map((op_code & 0x38) >> 3);
  register_value_set(dest, value);
}

void inst_load_r_hl(uint8_t op_code) {
  uint16_t hl_address;
  uint8_t dest;
  dest = register_map((op_code & 0x38) >> 3);
  hl_address = register_value_get(REG_HL);
  _load_r_from_mem(dest, hl_address);
}

void inst_load_r_idx(uint8_t op_code, uint8_t index_reg, uint8_t d) {
  uint16_t index_address;
  uint8_t dest;
  dest = register_map((op_code & 0x38) >> 3);
  index_address = register_value_get(index_reg);
  _load_r_from_mem(dest, index_address + d);
}

void inst_load_hl_r(uint8_t op_code) {
  uint8_t source_reg;
  uint16_t hl_address;
  uint8_t dest;
  uint8_t value;
  source_reg = register_map(op_code & 0x07);
  value = register_value_get(source_reg);
  hl_address = register_value_get(REG_HL);
  memory_set(hl_address, value);
}

void inst_load_idx_r(uint8_t op_code, uint8_t index_reg, uint8_t d) {
  uint16_t index_address;
  uint8_t source_reg;
  uint8_t dest;
  uint8_t value;
  source_reg = register_map((op_code & 0x07));
  index_address = register_value_get(index_reg);
  value = register_value_get(source_reg);
  memory_set(index_address + d, value);
}

void inst_load_hl_n(uint8_t value) {
  uint16_t hl_address;
  hl_address = register_value_get(REG_HL);
  memory_set(hl_address, value);
}

void inst_load_idx_n(uint8_t index_reg, uint8_t d, uint8_t value) {
  uint16_t index_address;
  index_address = register_value_get(index_reg);
  memory_set(index_address + d, value);
}

void inst_load_a_mem(uint16_t address) {
  uint8_t value;
  value = memory_get(address);
  register_value_set(REG_A, value);
}

void inst_load_a_rr(uint8_t rr) {
  uint16_t reg_address;
  reg_address = register_value_get(rr);
  inst_load_a_mem(reg_address);
}

void inst_load_mem_a(uint16_t address) {
  uint8_t value;
  value = register_value_get(REG_A);
  memory_set(address, value);
}
void inst_load_rr_a(uint8_t rr) {
  uint16_t reg_address;
  reg_address = register_value_get(rr);
  inst_load_mem_a(reg_address);
}
