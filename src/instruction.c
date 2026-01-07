#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "instruction.h"
#include "memory.h"
#include "register.h"

static instruction_map_t instruction_map[] = {
    {0x00, I_NOP, "NOP"},
    {0x02, I_LOAD_RR_A, "LD (BC/DE),A"},
    {0x06, I_LOAD_R_N, "LD r,n"},
    {0x0A, I_LOAD_RR_A, "LD (BC/DE),A"},
    {0x0E, I_LOAD_R_N, "LD r,n"},
    {0x12, I_LOAD_RR_A, "LD (BC/DE),A"},
    {0x16, I_LOAD_R_N, "LD r,n"},
    {0x1A, I_LOAD_RR_A, "LD (BC/DE),A"},
    {0x1E, I_LOAD_R_N, "LD r,n"},
    {0x26, I_LOAD_R_N, "LD r,n"},
    {0x2E, I_LOAD_R_N, "LD r,n"},
    {0x32, I_LOAD_MEM_A, "LD (nn),A"},
    {0x3A, I_LOAD_A_MEM, "LD A,(nn)"},
    {0x3E, I_LOAD_R_N, "LD r,n"},
    {0x36, I_LOAD_HL_N, "LD (HL),n"},
    {0x40, I_LOAD_R_R, "LD r,r"},
    {0x41, I_LOAD_R_R, "LD r,r"},
    {0x42, I_LOAD_R_R, "LD r,r"},
    {0x43, I_LOAD_R_R, "LD r,r"},
    {0x44, I_LOAD_R_R, "LD r,r"},
    {0x45, I_LOAD_R_R, "LD r,r"},
    {0x46, I_LOAD_HL_R, "LD (HL),r"},
    {0x47, I_LOAD_R_R, "LD r,r"},
    {0x48, I_LOAD_R_R, "LD r,r"},
    {0x49, I_LOAD_R_R, "LD r,r"},
    {0x4A, I_LOAD_R_R, "LD r,r"},
    {0x4B, I_LOAD_R_R, "LD r,r"},
    {0x4C, I_LOAD_R_R, "LD r,r"},
    {0x4D, I_LOAD_R_R, "LD r,r"},
    {0x4E, I_LOAD_HL_R, "LD (HL),r"},
    {0x4F, I_LOAD_R_R, "LD r,r"},
    {0x50, I_LOAD_R_R, "LD r,r"},
    {0x51, I_LOAD_R_R, "LD r,r"},
    {0x52, I_LOAD_R_R, "LD r,r"},
    {0x53, I_LOAD_R_R, "LD r,r"},
    {0x54, I_LOAD_R_R, "LD r,r"},
    {0x55, I_LOAD_R_R, "LD r,r"},
    {0x56, I_LOAD_HL_R, "LD (HL),r"},
    {0x57, I_LOAD_R_R, "LD r,r"},
    {0x58, I_LOAD_R_R, "LD r,r"},
    {0x59, I_LOAD_R_R, "LD r,r"},
    {0x5A, I_LOAD_R_R, "LD r,r"},
    {0x5B, I_LOAD_R_R, "LD r,r"},
    {0x5C, I_LOAD_R_R, "LD r,r"},
    {0x5D, I_LOAD_R_R, "LD r,r"},
    {0x5E, I_LOAD_HL_R, "LD (HL),r"},
    {0x5F, I_LOAD_R_R, "LD r,r"},
    {0x60, I_LOAD_R_R, "LD r,r"},
    {0x61, I_LOAD_R_R, "LD r,r"},
    {0x62, I_LOAD_R_R, "LD r,r"},
    {0x63, I_LOAD_R_R, "LD r,r"},
    {0x64, I_LOAD_R_R, "LD r,r"},
    {0x65, I_LOAD_R_R, "LD r,r"},
    {0x66, I_LOAD_HL_R, "LD (HL),r"},
    {0x67, I_LOAD_R_R, "LD r,r"},
    {0x68, I_LOAD_R_R, "LD r,r"},
    {0x69, I_LOAD_R_R, "LD r,r"},
    {0x6A, I_LOAD_R_R, "LD r,r"},
    {0x6B, I_LOAD_R_R, "LD r,r"},
    {0x6C, I_LOAD_R_R, "LD r,r"},
    {0x6D, I_LOAD_R_R, "LD r,r"},
    {0x6E, I_LOAD_HL_R, "LD (HL),r"},
    {0x6F, I_LOAD_R_R, "LD r,r"},
    {0x7E, I_LOAD_HL_R, "LD (HL),r"},
    {0x70, I_LOAD_HL_R, "LD (HL),r"},
    {0x71, I_LOAD_HL_R, "LD (HL),r"},
    {0x72, I_LOAD_HL_R, "LD (HL),r"},
    {0x73, I_LOAD_HL_R, "LD (HL),r"},
    {0x74, I_LOAD_HL_R, "LD (HL),r"},
    {0x75, I_LOAD_HL_R, "LD (HL),r"},
    {0x77, I_LOAD_HL_R, "LD (HL),r"},
    {0x78, I_LOAD_R_R, "LD r,r"},
    {0x79, I_LOAD_R_R, "LD r,r"},
    {0x7A, I_LOAD_R_R, "LD r,r"},
    {0x7B, I_LOAD_R_R, "LD r,r"},
    {0x7C, I_LOAD_R_R, "LD r,r"},
    {0x7D, I_LOAD_R_R, "LD r,r"},
    {0x7F, I_LOAD_R_R, "LD r,r"},
    {0xDD36, I_LOAD_IDX_N, "LD (IX/IY+d),n"},
    {0xDD46, I_LOAD_R_IDX, "LD r,(IX/IY+d)"},
    {0xDD4E, I_LOAD_R_IDX, "LD r,(IX/IY+d)"},
    {0xDD56, I_LOAD_R_IDX, "LD r,(IX/IY+d)"},
    {0xDD5E, I_LOAD_R_IDX, "LD r,(IX/IY+d)"},
    {0xDD66, I_LOAD_R_IDX, "LD r,(IX/IY+d)"},
    {0xDD6E, I_LOAD_R_IDX, "LD r,(IX/IY+d)"},
    {0xDD70, I_LOAD_IDX_R, "LD (IX/IY+d),r"},
    {0xDD71, I_LOAD_IDX_R, "LD (IX/IY+d),r"},
    {0xDD72, I_LOAD_IDX_R, "LD (IX/IY+d),r"},
    {0xDD73, I_LOAD_IDX_R, "LD (IX/IY+d),r"},
    {0xDD74, I_LOAD_IDX_R, "LD (IX/IY+d),r"},
    {0xDD75, I_LOAD_IDX_R, "LD (IX/IY+d),r"},
    {0xDD77, I_LOAD_IDX_R, "LD (IX/IY+d),r"},
    {0xDD7E, I_LOAD_IDX_R, "LD (IX/IY+d),r"},
    {0xDD7E, I_LOAD_R_IDX, "LD r,(IX/IY+d)"},
    {0xED47, I_LOAD_A_I, "LD A,I"},
    {0xED4F, I_LOAD_A_R_REG, "LD A,R"},
    {0xED57, I_LOAD_I_A, "LD I,A"},
    {0xED5F, I_LOAD_R_REG_A, "LD R,A"},
    {0xEDA0, I_BLKT, "LDI"},
    {0xEDA1, I_BLKS, "CPI"},
    {0xEDA8, I_BLKT, "LDD"},
    {0xEDA9, I_BLKS, "CPD"},
    {0xEDB0, I_BLKT, "LDIR"},
    {0xEDB1, I_BLKS, "CPIR"},
    {0xEDB8, I_BLKT, "LDDR"},
    {0xEDB9, I_BLKS, "CPDR"},
    {0XFD36, I_LOAD_IDX_N, "LD (IX/IY+d),n"},
    {0xFD46, I_LOAD_R_IDX, "LD r,(IX/IY+d)"},
    {0xFD4E, I_LOAD_R_IDX, "LD r,(IX/IY+d)"},
    {0xFD56, I_LOAD_R_IDX, "LD r,(IX/IY+d)"},
    {0xFD5E, I_LOAD_R_IDX, "LD r,(IX/IY+d)"},
    {0xFD66, I_LOAD_R_IDX, "LD r,(IX/IY+d)"},
    {0xFD6E, I_LOAD_R_IDX, "LD r,(IX/IY+d)"},
    {0xFD70, I_LOAD_IDX_R, "LD (IX/IY+d),r"},
    {0xFD71, I_LOAD_IDX_R, "LD (IX/IY+d),r"},
    {0xFD72, I_LOAD_IDX_R, "LD (IX/IY+d),r"},
    {0xFD73, I_LOAD_IDX_R, "LD (IX/IY+d),r"},
    {0xFD74, I_LOAD_IDX_R, "LD (IX/IY+d),r"},
    {0xFD75, I_LOAD_IDX_R, "LD (IX/IY+d),r"},
    {0xFD77, I_LOAD_IDX_R, "LD (IX/IY+d),r"},
    {0xFD7E, I_LOAD_IDX_R, "LD (IX/IY+d),r"},
    {0xFD7E, I_LOAD_R_IDX, "LD r,(IX/IY+d)"},
    {0xFF, I_U, "UNDEFINED"}};

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

void _load_mem_from_mem(uint16_t source, uint16_t dest) {
  memory_set(dest, memory_get(source));
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

void inst_blkt(uint16_t op_code) {
  uint8_t hl_value = 0;
  uint8_t a_value = 0;
  uint8_t repeat = 0;

  while (1) {
    hl_value = memory_get(register_value_get(REG_HL));
    a_value = register_value_get(REG_A);
    _load_mem_from_mem(register_value_get(REG_DE), register_value_get(REG_HL));

    if (op_code & 0x0008) {
      register_dec(REG_DE);
      register_dec(REG_HL);
    } else {
      register_inc(REG_DE);
      register_inc(REG_HL);
    }

    register_dec(REG_BC);

    // Flags: H and N reset, P/V set if BC != 0, S/Z/C unchanged.
    register_flag_unset(FLAG_H);
    register_flag_unset(FLAG_N);
    if (register_value_get(REG_BC) != 0) {
      register_flag_set(FLAG_PV);
    } else {
      register_flag_unset(FLAG_PV);
    }
    repeat = ((op_code & 0x00FF) == 0x00B0 || (op_code & 0x00FF) == 0x00B8);
    if (repeat && register_value_get(REG_BC) > 0) {
      continue;
    }

    break;
  }
}

void inst_blks(uint16_t op_code) {
  uint8_t a_value = 0;
  uint8_t hl_value = 0;
  uint8_t result = 0;
  uint16_t bc_value = 0;
  uint8_t repeat = 0;

  while (1) {
    a_value = register_value_get(REG_A);
    hl_value = memory_get(register_value_get(REG_HL));
    result = (uint8_t)(a_value - hl_value);

    // Flags from comparison
    if (result == 0) {
      register_flag_set(FLAG_Z);
    } else {
      register_flag_unset(FLAG_Z);
    }
    if (result & 0x80) {
      register_flag_set(FLAG_S);
    } else {
      register_flag_unset(FLAG_S);
    }
    if ((a_value & 0x0F) < (hl_value & 0x0F)) {
      register_flag_set(FLAG_H);
    } else {
      register_flag_unset(FLAG_H);
    }
    register_flag_set(FLAG_N);

    register_dec(REG_BC);
    bc_value = register_value_get(REG_BC);
    if (bc_value != 0) {
      register_flag_set(FLAG_PV);
    } else {
      register_flag_unset(FLAG_PV);
    }

    if (op_code & 0x0008) {
      register_dec(REG_HL);
    } else {
      register_inc(REG_HL);
    }

    repeat = ((op_code & 0x00FF) == 0x00B1 || (op_code & 0x00FF) == 0x00B9);
    if (repeat && bc_value > 0 && result != 0) {
      continue;
    }

    break;
  }
}
