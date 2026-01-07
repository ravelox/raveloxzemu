#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h" // IWYU pragma: keep
#include "instruction.h"
#include "memory.h"
#include "register.h"

static instruction_map_t instruction_map[] = {
    {0x00, I_NOP, "NOP"},
    {0x01, I_LOAD_RR_NN, "LD rr,nn"},
    {0x02, I_LOAD_RR_A, "LD (BC/DE),A"},
    {0x06, I_LOAD_R_N, "LD r,n"},
    {0x08, I_EX, "EX AF,AF'"},
    {0x0A, I_LOAD_RR_A, "LD (BC/DE),A"},
    {0x0E, I_LOAD_R_N, "LD r,n"},
    {0x11, I_LOAD_RR_NN, "LD rr,nn"},
    {0x12, I_LOAD_RR_A, "LD (BC/DE),A"},
    {0x16, I_LOAD_R_N, "LD r,n"},
    {0x1A, I_LOAD_RR_A, "LD (BC/DE),A"},
    {0x1E, I_LOAD_R_N, "LD r,n"},
    {0x21, I_LOAD_RR_NN, "LD rr,nn"},
    {0x26, I_LOAD_R_N, "LD r,n"},
    {0x2E, I_LOAD_R_N, "LD r,n"},
    {0x22, I_LOAD_MEM_RR, "LD (nn),rr"},
    {0x2A, I_LOAD_RR_MEM, "LD rr,(nn)"},
    {0x31, I_LOAD_RR_NN, "LD rr,nn"},
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
    {0x80, I_ADD_A_R, "ADD A,r"},
    {0x81, I_ADD_A_R, "ADD A,r"},
    {0x82, I_ADD_A_R, "ADD A,r"},
    {0x83, I_ADD_A_R, "ADD A,r"},
    {0x84, I_ADD_A_R, "ADD A,r"},
    {0x85, I_ADD_A_R, "ADD A,r"},
    {0x86, I_ADD_A_R, "ADD A,(HL)"},
    {0x87, I_ADD_A_R, "ADD A,r"},
    {0x88, I_ADC_A_R, "ADC A,r"},
    {0x89, I_ADC_A_R, "ADC A,r"},
    {0x8A, I_ADC_A_R, "ADC A,r"},
    {0x8B, I_ADC_A_R, "ADC A,r"},
    {0x8C, I_ADC_A_R, "ADC A,r"},
    {0x8D, I_ADC_A_R, "ADC A,r"},
    {0x8E, I_ADC_A_R, "ADC A,(HL)"},
    {0x8F, I_ADC_A_R, "ADC A,r"},
    {0x90, I_SUB_R, "SUB r"},
    {0x91, I_SUB_R, "SUB r"},
    {0x92, I_SUB_R, "SUB r"},
    {0x93, I_SUB_R, "SUB r"},
    {0x94, I_SUB_R, "SUB r"},
    {0x95, I_SUB_R, "SUB r"},
    {0x96, I_SUB_R, "SUB (HL)"},
    {0x97, I_SUB_R, "SUB r"},
    {0x98, I_SBC_A_R, "SBC A,r"},
    {0x99, I_SBC_A_R, "SBC A,r"},
    {0x9A, I_SBC_A_R, "SBC A,r"},
    {0x9B, I_SBC_A_R, "SBC A,r"},
    {0x9C, I_SBC_A_R, "SBC A,r"},
    {0x9D, I_SBC_A_R, "SBC A,r"},
    {0x9E, I_SBC_A_R, "SBC A,(HL)"},
    {0x9F, I_SBC_A_R, "SBC A,r"},
    {0xC6, I_ADD_A_N, "ADD A,n"},
    {0xCE, I_ADC_A_N, "ADC A,n"},
    {0xD6, I_SUB_N, "SUB n"},
    {0xDE, I_SBC_A_N, "SBC A,n"},
    {0x04, I_INC_R, "INC r"},
    {0x0C, I_INC_R, "INC r"},
    {0x14, I_INC_R, "INC r"},
    {0x1C, I_INC_R, "INC r"},
    {0x24, I_INC_R, "INC r"},
    {0x2C, I_INC_R, "INC r"},
    {0x3C, I_INC_R, "INC r"},
    {0x34, I_INC_HL, "INC (HL)"},
    {0x05, I_DEC_R, "DEC r"},
    {0x0D, I_DEC_R, "DEC r"},
    {0x15, I_DEC_R, "DEC r"},
    {0x1D, I_DEC_R, "DEC r"},
    {0x25, I_DEC_R, "DEC r"},
    {0x2D, I_DEC_R, "DEC r"},
    {0x3D, I_DEC_R, "DEC r"},
    {0x35, I_DEC_HL, "DEC (HL)"},
    {0xC1, I_POP, "POP rr"},
    {0xC5, I_PUSH, "PUSH rr"},
    {0xD1, I_POP, "POP rr"},
    {0xD5, I_PUSH, "PUSH rr"},
    {0xE1, I_POP, "POP rr"},
    {0xE5, I_PUSH, "PUSH rr"},
    {0xF1, I_POP, "POP rr"},
    {0xF5, I_PUSH, "PUSH rr"},
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
    {0xDD86, I_ADD_A_IDX, "ADD A,(IX/IY+d)"},
    {0xDD8E, I_ADC_A_IDX, "ADC A,(IX/IY+d)"},
    {0xDD96, I_SUB_IDX, "SUB (IX/IY+d)"},
    {0xDD9E, I_SBC_A_IDX, "SBC A,(IX/IY+d)"},
    {0xDD34, I_INC_IDX, "INC (IX/IY+d)"},
    {0xDD35, I_DEC_IDX, "DEC (IX/IY+d)"},
    {0xDDE1, I_POP, "POP rr"},
    {0xDDE5, I_PUSH, "PUSH rr"},
    {0xDD21, I_LOAD_RR_NN, "LD rr,nn"},
    {0xDD22, I_LOAD_MEM_RR, "LD (nn),rr"},
    {0xDD2A, I_LOAD_RR_MEM, "LD rr,(nn)"},
    {0xDDE3, I_EX, "EX (SP),IX"},
    {0xDDF9, I_LOAD_SP_RR, "LD SP,rr"},
    {0xE3, I_EX, "EX (SP),HL"},
    {0xEB, I_EX, "EX DE,HL"},
    {0xED47, I_LOAD_A_I, "LD A,I"},
    {0xED4F, I_LOAD_A_R_REG, "LD A,R"},
    {0xED57, I_LOAD_I_A, "LD I,A"},
    {0xED5F, I_LOAD_R_REG_A, "LD R,A"},
    {0xED73, I_LOAD_MEM_SP, "LD (nn),SP"},
    {0xED7B, I_LOAD_SP_MEM, "LD SP,(nn)"},
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
    {0xFD86, I_ADD_A_IDX, "ADD A,(IX/IY+d)"},
    {0xFD8E, I_ADC_A_IDX, "ADC A,(IX/IY+d)"},
    {0xFD96, I_SUB_IDX, "SUB (IX/IY+d)"},
    {0xFD9E, I_SBC_A_IDX, "SBC A,(IX/IY+d)"},
    {0xFD34, I_INC_IDX, "INC (IX/IY+d)"},
    {0xFD35, I_DEC_IDX, "DEC (IX/IY+d)"},
    {0xFD21, I_LOAD_RR_NN, "LD rr,nn"},
    {0xFD22, I_LOAD_MEM_RR, "LD (nn),rr"},
    {0xFD2A, I_LOAD_RR_MEM, "LD rr,(nn)"},
    {0xFDE3, I_EX, "EX (SP),IY"},
    {0xFDE1, I_POP, "POP rr"},
    {0xFDE5, I_PUSH, "PUSH rr"},
    {0xFDF9, I_LOAD_SP_RR, "LD SP,rr"},
    {0xF9, I_LOAD_SP_RR, "LD SP,rr"},
    {0xFF, I_U, "UNDEFINED"}};

static const char *register_name_8(uint8_t reg) {
  switch (reg) {
  case REG_A:
    return "A";
  case REG_B:
    return "B";
  case REG_C:
    return "C";
  case REG_D:
    return "D";
  case REG_E:
    return "E";
  case REG_H:
    return "H";
  case REG_L:
    return "L";
  default:
    return "?";
  }
}

static const char *register_name_16(uint8_t reg) {
  switch (reg) {
  case REG_AF:
    return "AF";
  case REG_BC:
    return "BC";
  case REG_DE:
    return "DE";
  case REG_HL:
    return "HL";
  case REG_IX:
    return "IX";
  case REG_IY:
    return "IY";
  case REG_SP:
    return "SP";
  default:
    return "?";
  }
}

static void instruction_log(cpu_t *cpu, const char *format, ...) {
  va_list args;

  if (!cpu)
    return;

  va_start(args, format);
  vsnprintf(cpu->last_instruction, sizeof(cpu->last_instruction), format, args);
  va_end(args);
}

static void flag_set(cpu_t *cpu, uint8_t flag, int condition) {
  if (condition)
    register_flag_set(cpu, flag);
  else
    register_flag_unset(cpu, flag);
}

static uint8_t read_r_value(cpu_t *cpu, uint8_t op_code, const char **label) {
  uint8_t r_bits = op_code & 0x07;
  if (r_bits == 0x06) {
    if (label)
      *label = "(HL)";
    return memory_get(cpu, register_value_get(cpu, REG_HL));
  }
  if (label)
    *label = register_name_8(register_map(r_bits));
  return (uint8_t)register_value_get(cpu, register_map(r_bits));
}

static void update_flags_add(cpu_t *cpu, uint8_t a, uint8_t value,
                             uint8_t carry, uint8_t result, uint16_t sum) {
  flag_set(cpu, FLAG_S, result & 0x80);
  flag_set(cpu, FLAG_Z, result == 0);
  flag_set(cpu, FLAG_H,
           ((a & 0x0F) + (value & 0x0F) + carry) > 0x0F);
  flag_set(cpu, FLAG_PV, (~(a ^ value) & (a ^ result) & 0x80) != 0);
  register_flag_unset(cpu, FLAG_N);
  flag_set(cpu, FLAG_C, sum > 0xFF);
}

static void update_flags_sub(cpu_t *cpu, uint8_t a, uint8_t value,
                             uint8_t carry, uint8_t result, int16_t diff) {
  flag_set(cpu, FLAG_S, result & 0x80);
  flag_set(cpu, FLAG_Z, result == 0);
  flag_set(cpu, FLAG_H, (a & 0x0F) < ((value & 0x0F) + carry));
  flag_set(cpu, FLAG_PV, ((a ^ value) & (a ^ result) & 0x80) != 0);
  register_flag_set(cpu, FLAG_N);
  flag_set(cpu, FLAG_C, diff < 0);
}

static uint8_t inc_value(cpu_t *cpu, uint8_t value) {
  uint8_t result = (uint8_t)(value + 1);
  flag_set(cpu, FLAG_S, result & 0x80);
  flag_set(cpu, FLAG_Z, result == 0);
  flag_set(cpu, FLAG_H, ((value & 0x0F) + 1) > 0x0F);
  flag_set(cpu, FLAG_PV, value == 0x7F);
  register_flag_unset(cpu, FLAG_N);
  return result;
}

static uint8_t dec_value(cpu_t *cpu, uint8_t value) {
  uint8_t result = (uint8_t)(value - 1);
  flag_set(cpu, FLAG_S, result & 0x80);
  flag_set(cpu, FLAG_Z, result == 0);
  flag_set(cpu, FLAG_H, (value & 0x0F) == 0x00);
  flag_set(cpu, FLAG_PV, value == 0x80);
  register_flag_set(cpu, FLAG_N);
  return result;
}

instruction_group_t instruction_group_get(uint16_t op_code) {
  for (uint16_t index = 0; index < sizeof(instruction_map); index++) {
    if (instruction_map[index].op_code == op_code)
      return instruction_map[index].group;
  }

  return I_U;
}

void _load_r_r(cpu_t *cpu, uint8_t source, uint8_t dest) {
  uint8_t value = register_value_get(cpu,source);
  register_value_set(cpu,dest, value);
}

void _load_r_from_mem(cpu_t *cpu, uint8_t reg, uint16_t address) {
  uint8_t value = memory_get(cpu,address);
  register_value_set(cpu,reg, value);
}

void _load_mem_from_mem(cpu_t *cpu, uint16_t source, uint16_t dest) {
  memory_set(cpu,dest, memory_get(cpu,source));
}

uint16_t _load_word_from_mem(cpu_t *cpu, uint16_t address) {
  uint8_t low = memory_get(cpu,address);
  uint8_t high = memory_get(cpu,(uint16_t)(address + 1));
  return (uint16_t)((high << 8) | low);
}

void _store_word_to_mem(cpu_t *cpu, uint16_t address, uint16_t value) {
  memory_set(cpu,address, (uint8_t)(value & 0x00FF));
  memory_set(cpu,(uint16_t)(address + 1), (uint8_t)((value >> 8) & 0x00FF));
}

void inst_load_r_r(cpu_t *cpu, uint8_t op_code) {
  uint8_t source, dest;
  dest = register_map((op_code & 0x38) >> 3);
  source = register_map(op_code & 0x7);
  instruction_log(cpu, "LD %s,%s", register_name_8(dest),
                  register_name_8(source));
  _load_r_r(cpu,source, dest);
}

void inst_load_r_n(cpu_t *cpu, uint8_t op_code, uint8_t value) {
  uint8_t dest;
  dest = register_map((op_code & 0x38) >> 3);
  instruction_log(cpu, "LD %s,0x%02X", register_name_8(dest), value);
  register_value_set(cpu,dest, value);
}

void inst_load_r_hl(cpu_t *cpu, uint8_t op_code) {
  uint16_t hl_address;
  uint8_t dest;
  dest = register_map((op_code & 0x38) >> 3);
  instruction_log(cpu, "LD %s,(HL)", register_name_8(dest));
  hl_address = register_value_get(cpu,REG_HL);
  _load_r_from_mem(cpu,dest, hl_address);
}

void inst_load_r_idx(cpu_t *cpu, uint8_t op_code, uint8_t index_reg,
                     uint8_t d) {
  uint16_t index_address;
  uint8_t dest;
  dest = register_map((op_code & 0x38) >> 3);
  instruction_log(cpu, "LD %s,(%s%+d)", register_name_8(dest),
                  register_name_16(index_reg), (int8_t)d);
  index_address = register_value_get(cpu,index_reg);
  _load_r_from_mem(cpu,dest, index_address + d);
}

void inst_load_hl_r(cpu_t *cpu, uint8_t op_code) {
  uint8_t source_reg;
  uint16_t hl_address;
  uint8_t dest;
  uint8_t value;
  source_reg = register_map(op_code & 0x07);
  instruction_log(cpu, "LD (HL),%s", register_name_8(source_reg));
  value = register_value_get(cpu,source_reg);
  hl_address = register_value_get(cpu,REG_HL);
  memory_set(cpu,hl_address, value);
}

void inst_load_idx_r(cpu_t *cpu, uint8_t op_code, uint8_t index_reg,
                     uint8_t d) {
  uint16_t index_address;
  uint8_t source_reg;
  uint8_t dest;
  uint8_t value;
  source_reg = register_map((op_code & 0x07));
  instruction_log(cpu, "LD (%s%+d),%s", register_name_16(index_reg),
                  (int8_t)d,
                  register_name_8(source_reg));
  index_address = register_value_get(cpu,index_reg);
  value = register_value_get(cpu,source_reg);
  memory_set(cpu,index_address + d, value);
}

void inst_load_hl_n(cpu_t *cpu, uint8_t value) {
  uint16_t hl_address;
  instruction_log(cpu, "LD (HL),0x%02X", value);
  hl_address = register_value_get(cpu,REG_HL);
  memory_set(cpu,hl_address, value);
}

void inst_load_idx_n(cpu_t *cpu, uint8_t index_reg, uint8_t d, uint8_t value) {
  uint16_t index_address;
  instruction_log(cpu, "LD (%s%+d),0x%02X", register_name_16(index_reg),
                  (int8_t)d, value);
  index_address = register_value_get(cpu,index_reg);
  memory_set(cpu,index_address + d, value);
}

void inst_load_a_mem(cpu_t *cpu, uint16_t address) {
  uint8_t value;
  instruction_log(cpu, "LD A,(0x%04X)", address);
  value = memory_get(cpu,address);
  register_value_set(cpu,REG_A, value);
}

void inst_load_a_rr(cpu_t *cpu, uint8_t rr) {
  uint16_t reg_address;
  instruction_log(cpu, "LD A,(%s)", register_name_16(rr));
  reg_address = register_value_get(cpu,rr);
  inst_load_a_mem(cpu, reg_address);
}

void inst_load_mem_a(cpu_t *cpu, uint16_t address) {
  uint8_t value;
  instruction_log(cpu, "LD (0x%04X),A", address);
  value = register_value_get(cpu,REG_A);
  memory_set(cpu,address, value);
}
void inst_load_rr_a(cpu_t *cpu, uint8_t rr) {
  uint16_t reg_address;
  instruction_log(cpu, "LD (%s),A", register_name_16(rr));
  reg_address = register_value_get(cpu,rr);
  inst_load_mem_a(cpu, reg_address);
}

void inst_load_rr_nn(cpu_t *cpu, uint8_t rr, uint16_t value) {
  instruction_log(cpu, "LD %s,0x%04X", register_name_16(rr), value);
  register_value_set(cpu,rr, value);
}

void inst_load_rr_mem(cpu_t *cpu, uint8_t rr, uint16_t address) {
  instruction_log(cpu, "LD %s,(0x%04X)", register_name_16(rr), address);
  register_value_set(cpu,rr, _load_word_from_mem(cpu,address));
}

void inst_load_mem_rr(cpu_t *cpu, uint8_t rr, uint16_t address) {
  instruction_log(cpu, "LD (0x%04X),%s", address, register_name_16(rr));
  _store_word_to_mem(cpu,address, register_value_get(cpu,rr));
}

void inst_load_sp_rr(cpu_t *cpu, uint8_t rr) {
  instruction_log(cpu, "LD SP,%s", register_name_16(rr));
  register_value_set(cpu,REG_SP, register_value_get(cpu,rr));
}

void inst_load_sp_mem(cpu_t *cpu, uint16_t address) {
  instruction_log(cpu, "LD SP,(0x%04X)", address);
  register_value_set(cpu,REG_SP, _load_word_from_mem(cpu,address));
}

void inst_load_mem_sp(cpu_t *cpu, uint16_t address) {
  instruction_log(cpu, "LD (0x%04X),SP", address);
  _store_word_to_mem(cpu,address, register_value_get(cpu,REG_SP));
}

void inst_push_rr(cpu_t *cpu, uint8_t rr) {
  uint16_t sp = register_value_get(cpu,REG_SP);
  uint16_t value = register_value_get(cpu,rr);
  instruction_log(cpu, "PUSH %s", register_name_16(rr));

  sp--;
  memory_set(cpu,sp, (uint8_t)((value >> 8) & 0x00FF));
  sp--;
  memory_set(cpu,sp, (uint8_t)(value & 0x00FF));

  register_value_set(cpu,REG_SP, sp);
}

void inst_pop_rr(cpu_t *cpu, uint8_t rr) {
  uint16_t sp = register_value_get(cpu,REG_SP);
  uint8_t low = memory_get(cpu,sp);
  sp++;
  uint8_t high = memory_get(cpu,sp);
  sp++;

  instruction_log(cpu, "POP %s", register_name_16(rr));
  register_value_set(cpu,rr, (uint16_t)((high << 8) | low));
  register_value_set(cpu,REG_SP, sp);
}

void inst_add_a_r(cpu_t *cpu, uint8_t op_code) {
  const char *label = NULL;
  uint8_t value = read_r_value(cpu, op_code, &label);
  uint8_t a = (uint8_t)register_value_get(cpu, REG_A);
  uint16_t sum = (uint16_t)(a + value);
  uint8_t result = (uint8_t)sum;

  instruction_log(cpu, "ADD A,%s", label ? label : "?");
  register_value_set(cpu, REG_A, result);
  update_flags_add(cpu, a, value, 0, result, sum);
}

void inst_add_a_n(cpu_t *cpu, uint8_t value) {
  uint8_t a = (uint8_t)register_value_get(cpu, REG_A);
  uint16_t sum = (uint16_t)(a + value);
  uint8_t result = (uint8_t)sum;

  instruction_log(cpu, "ADD A,0x%02X", value);
  register_value_set(cpu, REG_A, result);
  update_flags_add(cpu, a, value, 0, result, sum);
}

void inst_add_a_idx(cpu_t *cpu, uint8_t index_reg, uint8_t d) {
  uint16_t address = register_value_get(cpu, index_reg);
  uint8_t value = memory_get(cpu, (uint16_t)(address + d));
  uint8_t a = (uint8_t)register_value_get(cpu, REG_A);
  uint16_t sum = (uint16_t)(a + value);
  uint8_t result = (uint8_t)sum;

  instruction_log(cpu, "ADD A,(%s%+d)", register_name_16(index_reg),
                  (int8_t)d);
  register_value_set(cpu, REG_A, result);
  update_flags_add(cpu, a, value, 0, result, sum);
}

void inst_adc_a_r(cpu_t *cpu, uint8_t op_code) {
  const char *label = NULL;
  uint8_t value = read_r_value(cpu, op_code, &label);
  uint8_t a = (uint8_t)register_value_get(cpu, REG_A);
  uint8_t carry = (register_value_get(cpu, REG_F) & (1 << FLAG_C)) ? 1 : 0;
  uint16_t sum = (uint16_t)(a + value + carry);
  uint8_t result = (uint8_t)sum;

  instruction_log(cpu, "ADC A,%s", label ? label : "?");
  register_value_set(cpu, REG_A, result);
  update_flags_add(cpu, a, value, carry, result, sum);
}

void inst_adc_a_n(cpu_t *cpu, uint8_t value) {
  uint8_t a = (uint8_t)register_value_get(cpu, REG_A);
  uint8_t carry = (register_value_get(cpu, REG_F) & (1 << FLAG_C)) ? 1 : 0;
  uint16_t sum = (uint16_t)(a + value + carry);
  uint8_t result = (uint8_t)sum;

  instruction_log(cpu, "ADC A,0x%02X", value);
  register_value_set(cpu, REG_A, result);
  update_flags_add(cpu, a, value, carry, result, sum);
}

void inst_adc_a_idx(cpu_t *cpu, uint8_t index_reg, uint8_t d) {
  uint16_t address = register_value_get(cpu, index_reg);
  uint8_t value = memory_get(cpu, (uint16_t)(address + d));
  uint8_t a = (uint8_t)register_value_get(cpu, REG_A);
  uint8_t carry = (register_value_get(cpu, REG_F) & (1 << FLAG_C)) ? 1 : 0;
  uint16_t sum = (uint16_t)(a + value + carry);
  uint8_t result = (uint8_t)sum;

  instruction_log(cpu, "ADC A,(%s%+d)", register_name_16(index_reg),
                  (int8_t)d);
  register_value_set(cpu, REG_A, result);
  update_flags_add(cpu, a, value, carry, result, sum);
}

void inst_sub_r(cpu_t *cpu, uint8_t op_code) {
  const char *label = NULL;
  uint8_t value = read_r_value(cpu, op_code, &label);
  uint8_t a = (uint8_t)register_value_get(cpu, REG_A);
  int16_t diff = (int16_t)a - (int16_t)value;
  uint8_t result = (uint8_t)diff;

  instruction_log(cpu, "SUB %s", label ? label : "?");
  register_value_set(cpu, REG_A, result);
  update_flags_sub(cpu, a, value, 0, result, diff);
}

void inst_sub_n(cpu_t *cpu, uint8_t value) {
  uint8_t a = (uint8_t)register_value_get(cpu, REG_A);
  int16_t diff = (int16_t)a - (int16_t)value;
  uint8_t result = (uint8_t)diff;

  instruction_log(cpu, "SUB 0x%02X", value);
  register_value_set(cpu, REG_A, result);
  update_flags_sub(cpu, a, value, 0, result, diff);
}

void inst_sub_idx(cpu_t *cpu, uint8_t index_reg, uint8_t d) {
  uint16_t address = register_value_get(cpu, index_reg);
  uint8_t value = memory_get(cpu, (uint16_t)(address + d));
  uint8_t a = (uint8_t)register_value_get(cpu, REG_A);
  int16_t diff = (int16_t)a - (int16_t)value;
  uint8_t result = (uint8_t)diff;

  instruction_log(cpu, "SUB (%s%+d)", register_name_16(index_reg),
                  (int8_t)d);
  register_value_set(cpu, REG_A, result);
  update_flags_sub(cpu, a, value, 0, result, diff);
}

void inst_sbc_a_r(cpu_t *cpu, uint8_t op_code) {
  const char *label = NULL;
  uint8_t value = read_r_value(cpu, op_code, &label);
  uint8_t a = (uint8_t)register_value_get(cpu, REG_A);
  uint8_t carry = (register_value_get(cpu, REG_F) & (1 << FLAG_C)) ? 1 : 0;
  int16_t diff = (int16_t)a - (int16_t)value - (int16_t)carry;
  uint8_t result = (uint8_t)diff;

  instruction_log(cpu, "SBC A,%s", label ? label : "?");
  register_value_set(cpu, REG_A, result);
  update_flags_sub(cpu, a, value, carry, result, diff);
}

void inst_sbc_a_n(cpu_t *cpu, uint8_t value) {
  uint8_t a = (uint8_t)register_value_get(cpu, REG_A);
  uint8_t carry = (register_value_get(cpu, REG_F) & (1 << FLAG_C)) ? 1 : 0;
  int16_t diff = (int16_t)a - (int16_t)value - (int16_t)carry;
  uint8_t result = (uint8_t)diff;

  instruction_log(cpu, "SBC A,0x%02X", value);
  register_value_set(cpu, REG_A, result);
  update_flags_sub(cpu, a, value, carry, result, diff);
}

void inst_sbc_a_idx(cpu_t *cpu, uint8_t index_reg, uint8_t d) {
  uint16_t address = register_value_get(cpu, index_reg);
  uint8_t value = memory_get(cpu, (uint16_t)(address + d));
  uint8_t a = (uint8_t)register_value_get(cpu, REG_A);
  uint8_t carry = (register_value_get(cpu, REG_F) & (1 << FLAG_C)) ? 1 : 0;
  int16_t diff = (int16_t)a - (int16_t)value - (int16_t)carry;
  uint8_t result = (uint8_t)diff;

  instruction_log(cpu, "SBC A,(%s%+d)", register_name_16(index_reg),
                  (int8_t)d);
  register_value_set(cpu, REG_A, result);
  update_flags_sub(cpu, a, value, carry, result, diff);
}

void inst_inc_r(cpu_t *cpu, uint8_t op_code) {
  uint8_t reg = register_map((op_code & 0x38) >> 3);
  uint8_t value = (uint8_t)register_value_get(cpu, reg);
  uint8_t result = inc_value(cpu, value);

  instruction_log(cpu, "INC %s", register_name_8(reg));
  register_value_set(cpu, reg, result);
}

void inst_inc_hl(cpu_t *cpu) {
  uint16_t address = register_value_get(cpu, REG_HL);
  uint8_t value = memory_get(cpu, address);
  uint8_t result = inc_value(cpu, value);

  instruction_log(cpu, "INC (HL)");
  memory_set(cpu, address, result);
}

void inst_inc_idx(cpu_t *cpu, uint8_t index_reg, uint8_t d) {
  uint16_t address = register_value_get(cpu, index_reg);
  uint16_t target = (uint16_t)(address + d);
  uint8_t value = memory_get(cpu, target);
  uint8_t result = inc_value(cpu, value);

  instruction_log(cpu, "INC (%s%+d)", register_name_16(index_reg),
                  (int8_t)d);
  memory_set(cpu, target, result);
}

void inst_dec_r(cpu_t *cpu, uint8_t op_code) {
  uint8_t reg = register_map((op_code & 0x38) >> 3);
  uint8_t value = (uint8_t)register_value_get(cpu, reg);
  uint8_t result = dec_value(cpu, value);

  instruction_log(cpu, "DEC %s", register_name_8(reg));
  register_value_set(cpu, reg, result);
}

void inst_dec_hl(cpu_t *cpu) {
  uint16_t address = register_value_get(cpu, REG_HL);
  uint8_t value = memory_get(cpu, address);
  uint8_t result = dec_value(cpu, value);

  instruction_log(cpu, "DEC (HL)");
  memory_set(cpu, address, result);
}

void inst_dec_idx(cpu_t *cpu, uint8_t index_reg, uint8_t d) {
  uint16_t address = register_value_get(cpu, index_reg);
  uint16_t target = (uint16_t)(address + d);
  uint8_t value = memory_get(cpu, target);
  uint8_t result = dec_value(cpu, value);

  instruction_log(cpu, "DEC (%s%+d)", register_name_16(index_reg),
                  (int8_t)d);
  memory_set(cpu, target, result);
}

void inst_blkt(cpu_t *cpu, uint16_t op_code) {
  uint8_t hl_value = 0;
  uint8_t a_value = 0;
  uint8_t repeat = 0;
  uint8_t op = (uint8_t)(op_code & 0x00FF);

  if (op == 0xA0) {
    instruction_log(cpu, "LDI");
  } else if (op == 0xA8) {
    instruction_log(cpu, "LDD");
  } else if (op == 0xB0) {
    instruction_log(cpu, "LDIR");
  } else if (op == 0xB8) {
    instruction_log(cpu, "LDDR");
  } else {
    instruction_log(cpu, "LDI/LDD/LDIR/LDDR");
  }

  while (1) {
    hl_value = memory_get(cpu,register_value_get(cpu,REG_HL));
    a_value = register_value_get(cpu,REG_A);
    _load_mem_from_mem(cpu,register_value_get(cpu,REG_DE), register_value_get(cpu,REG_HL));

    if (op_code & 0x0008) {
      register_dec(cpu,REG_DE);
      register_dec(cpu,REG_HL);
    } else {
      register_inc(cpu,REG_DE);
      register_inc(cpu,REG_HL);
    }

    register_dec(cpu,REG_BC);

    // Flags: H and N reset, P/V set if BC != 0, S/Z/C unchanged.
    register_flag_unset(cpu,FLAG_H);
    register_flag_unset(cpu,FLAG_N);
    if (register_value_get(cpu,REG_BC) != 0) {
      register_flag_set(cpu,FLAG_PV);
    } else {
      register_flag_unset(cpu,FLAG_PV);
    }
    repeat = ((op_code & 0x00FF) == 0x00B0 || (op_code & 0x00FF) == 0x00B8);
    if (repeat && register_value_get(cpu,REG_BC) > 0) {
      continue;
    }

    break;
  }
}

void inst_blks(cpu_t *cpu, uint16_t op_code) {
  uint8_t a_value = 0;
  uint8_t hl_value = 0;
  uint8_t result = 0;
  uint16_t bc_value = 0;
  uint8_t repeat = 0;
  uint8_t op = (uint8_t)(op_code & 0x00FF);

  if (op == 0xA1) {
    instruction_log(cpu, "CPI");
  } else if (op == 0xA9) {
    instruction_log(cpu, "CPD");
  } else if (op == 0xB1) {
    instruction_log(cpu, "CPIR");
  } else if (op == 0xB9) {
    instruction_log(cpu, "CPDR");
  } else {
    instruction_log(cpu, "CPI/CPD/CPIR/CPDR");
  }

  while (1) {
    a_value = register_value_get(cpu,REG_A);
    hl_value = memory_get(cpu,register_value_get(cpu,REG_HL));
    result = (uint8_t)(a_value - hl_value);

    // Flags from comparison
    if (result == 0) {
      register_flag_set(cpu,FLAG_Z);
    } else {
      register_flag_unset(cpu,FLAG_Z);
    }
    if (result & 0x80) {
      register_flag_set(cpu,FLAG_S);
    } else {
      register_flag_unset(cpu,FLAG_S);
    }
    if ((a_value & 0x0F) < (hl_value & 0x0F)) {
      register_flag_set(cpu,FLAG_H);
    } else {
      register_flag_unset(cpu,FLAG_H);
    }
    register_flag_set(cpu,FLAG_N);

    register_dec(cpu,REG_BC);
    bc_value = register_value_get(cpu,REG_BC);
    if (bc_value != 0) {
      register_flag_set(cpu,FLAG_PV);
    } else {
      register_flag_unset(cpu,FLAG_PV);
    }

    if (op_code & 0x0008) {
      register_dec(cpu,REG_HL);
    } else {
      register_inc(cpu,REG_HL);
    }

    repeat = ((op_code & 0x00FF) == 0x00B1 || (op_code & 0x00FF) == 0x00B9);
    if (repeat && bc_value > 0 && result != 0) {
      continue;
    }

    break;
  }
}
