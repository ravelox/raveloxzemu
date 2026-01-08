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
    {0x18, I_JR, "JR d"},
    {0x21, I_LOAD_RR_NN, "LD rr,nn"},
    {0x26, I_LOAD_R_N, "LD r,n"},
    {0x2E, I_LOAD_R_N, "LD r,n"},
    {0x27, I_DAA, "DAA"},
    {0x22, I_LOAD_MEM_RR, "LD (nn),rr"},
    {0x2A, I_LOAD_RR_MEM, "LD rr,(nn)"},
    {0x31, I_LOAD_RR_NN, "LD rr,nn"},
    {0x32, I_LOAD_MEM_A, "LD (nn),A"},
    {0x3A, I_LOAD_A_MEM, "LD A,(nn)"},
    {0x3E, I_LOAD_R_N, "LD r,n"},
    {0x36, I_LOAD_HL_N, "LD (HL),n"},
    {0x2F, I_CPL, "CPL"},
    {0x20, I_JR, "JR NZ,d"},
    {0x28, I_JR, "JR Z,d"},
    {0x30, I_JR, "JR NC,d"},
    {0x38, I_JR, "JR C,d"},
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
    {0x76, I_HALT, "HALT"},
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
    {0xA0, I_AND_R, "AND r"},
    {0xA1, I_AND_R, "AND r"},
    {0xA2, I_AND_R, "AND r"},
    {0xA3, I_AND_R, "AND r"},
    {0xA4, I_AND_R, "AND r"},
    {0xA5, I_AND_R, "AND r"},
    {0xA6, I_AND_R, "AND (HL)"},
    {0xA7, I_AND_R, "AND r"},
    {0xA8, I_XOR_R, "XOR r"},
    {0xA9, I_XOR_R, "XOR r"},
    {0xAA, I_XOR_R, "XOR r"},
    {0xAB, I_XOR_R, "XOR r"},
    {0xAC, I_XOR_R, "XOR r"},
    {0xAD, I_XOR_R, "XOR r"},
    {0xAE, I_XOR_R, "XOR (HL)"},
    {0xAF, I_XOR_R, "XOR r"},
    {0xB0, I_OR_R, "OR r"},
    {0xB1, I_OR_R, "OR r"},
    {0xB2, I_OR_R, "OR r"},
    {0xB3, I_OR_R, "OR r"},
    {0xB4, I_OR_R, "OR r"},
    {0xB5, I_OR_R, "OR r"},
    {0xB6, I_OR_R, "OR (HL)"},
    {0xB7, I_OR_R, "OR r"},
    {0xB8, I_CP_R, "CP r"},
    {0xB9, I_CP_R, "CP r"},
    {0xBA, I_CP_R, "CP r"},
    {0xBB, I_CP_R, "CP r"},
    {0xBC, I_CP_R, "CP r"},
    {0xBD, I_CP_R, "CP r"},
    {0xBE, I_CP_R, "CP (HL)"},
    {0xBF, I_CP_R, "CP r"},
    {0xC0, I_RET, "RET NZ"},
    {0xC2, I_JP, "JP NZ,nn"},
    {0xC3, I_JP, "JP nn"},
    {0xC4, I_CALL, "CALL NZ,nn"},
    {0xC6, I_ADD_A_N, "ADD A,n"},
    {0xC7, I_RST, "RST 00"},
    {0xC8, I_RET, "RET Z"},
    {0xC9, I_RET, "RET"},
    {0xCA, I_JP, "JP Z,nn"},
    {0xCC, I_CALL, "CALL Z,nn"},
    {0xCD, I_CALL, "CALL nn"},
    {0xCE, I_ADC_A_N, "ADC A,n"},
    {0xD6, I_SUB_N, "SUB n"},
    {0xD0, I_RET, "RET NC"},
    {0xD2, I_JP, "JP NC,nn"},
    {0xD4, I_CALL, "CALL NC,nn"},
    {0xDE, I_SBC_A_N, "SBC A,n"},
    {0xD7, I_RST, "RST 10"},
    {0xD8, I_RET, "RET C"},
    {0xDA, I_JP, "JP C,nn"},
    {0xDC, I_CALL, "CALL C,nn"},
    {0xDF, I_RST, "RST 18"},
    {0xE0, I_RET, "RET PO"},
    {0xE2, I_JP, "JP PO,nn"},
    {0xE4, I_CALL, "CALL PO,nn"},
    {0xE7, I_RST, "RST 20"},
    {0xE8, I_RET, "RET PE"},
    {0xE9, I_JP, "JP (HL)"},
    {0xEA, I_JP, "JP PE,nn"},
    {0xEC, I_CALL, "CALL PE,nn"},
    {0xEF, I_RST, "RST 28"},
    {0xF0, I_RET, "RET P"},
    {0xF2, I_JP, "JP P,nn"},
    {0xF4, I_CALL, "CALL P,nn"},
    {0xF7, I_RST, "RST 30"},
    {0xF8, I_RET, "RET M"},
    {0xFA, I_JP, "JP M,nn"},
    {0xFC, I_CALL, "CALL M,nn"},
    {0xFF, I_RST, "RST 38"},
    {0xE6, I_AND_N, "AND n"},
    {0xEE, I_XOR_N, "XOR n"},
    {0xF6, I_OR_N, "OR n"},
    {0xFE, I_CP_N, "CP n"},
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
    {0x37, I_SCF, "SCF"},
    {0x3F, I_CCF, "CCF"},
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
    {0xDDA6, I_AND_IDX, "AND (IX/IY+d)"},
    {0xDDAE, I_XOR_IDX, "XOR (IX/IY+d)"},
    {0xDDB6, I_OR_IDX, "OR (IX/IY+d)"},
    {0xDDBE, I_CP_IDX, "CP (IX/IY+d)"},
    {0xDD34, I_INC_IDX, "INC (IX/IY+d)"},
    {0xDD35, I_DEC_IDX, "DEC (IX/IY+d)"},
    {0xDDE1, I_POP, "POP rr"},
    {0xDDE5, I_PUSH, "PUSH rr"},
    {0xDD21, I_LOAD_RR_NN, "LD rr,nn"},
    {0xDD22, I_LOAD_MEM_RR, "LD (nn),rr"},
    {0xDD2A, I_LOAD_RR_MEM, "LD rr,(nn)"},
    {0xDDE3, I_EX, "EX (SP),IX"},
    {0xDDE9, I_JP, "JP (IX)"},
    {0xDDF9, I_LOAD_SP_RR, "LD SP,rr"},
    {0xE3, I_EX, "EX (SP),HL"},
    {0xEB, I_EX, "EX DE,HL"},
    {0xED47, I_LOAD_A_I, "LD A,I"},
    {0xED4F, I_LOAD_A_R_REG, "LD A,R"},
    {0xED57, I_LOAD_I_A, "LD I,A"},
    {0xED5F, I_LOAD_R_REG_A, "LD R,A"},
    {0xED44, I_NEG, "NEG"},
    {0xED4C, I_NEG, "NEG"},
    {0xED54, I_NEG, "NEG"},
    {0xED5C, I_NEG, "NEG"},
    {0xED64, I_NEG, "NEG"},
    {0xED6C, I_NEG, "NEG"},
    {0xED74, I_NEG, "NEG"},
    {0xED7C, I_NEG, "NEG"},
    {0xED45, I_RET, "RETN"},
    {0xED4D, I_RET, "RETI"},
    {0xED55, I_RET, "RETN"},
    {0xED5D, I_RET, "RETI"},
    {0xED65, I_RET, "RETN"},
    {0xED6D, I_RET, "RETI"},
    {0xED75, I_RET, "RETN"},
    {0xED7D, I_RET, "RETI"},
    {0xED46, I_IM, "IM 0"},
    {0xED4E, I_IM, "IM 0"},
    {0xED66, I_IM, "IM 0"},
    {0xED6E, I_IM, "IM 0"},
    {0xED56, I_IM, "IM 1"},
    {0xED76, I_IM, "IM 1"},
    {0xED5E, I_IM, "IM 2"},
    {0xED7E, I_IM, "IM 2"},
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
    {0xFDA6, I_AND_IDX, "AND (IX/IY+d)"},
    {0xFDAE, I_XOR_IDX, "XOR (IX/IY+d)"},
    {0xFDB6, I_OR_IDX, "OR (IX/IY+d)"},
    {0xFDBE, I_CP_IDX, "CP (IX/IY+d)"},
    {0xFD34, I_INC_IDX, "INC (IX/IY+d)"},
    {0xFD35, I_DEC_IDX, "DEC (IX/IY+d)"},
    {0xFD21, I_LOAD_RR_NN, "LD rr,nn"},
    {0xFD22, I_LOAD_MEM_RR, "LD (nn),rr"},
    {0xFD2A, I_LOAD_RR_MEM, "LD rr,(nn)"},
    {0xFDE3, I_EX, "EX (SP),IY"},
    {0xFDE9, I_JP, "JP (IY)"},
    {0xFDE1, I_POP, "POP rr"},
    {0xFDE5, I_PUSH, "PUSH rr"},
    {0xFDF9, I_LOAD_SP_RR, "LD SP,rr"},
    {0xF9, I_LOAD_SP_RR, "LD SP,rr"},
    {0xF3, I_DI, "DI"},
    {0xFB, I_EI, "EI"}};

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

static int parity_even(uint8_t value) {
  value ^= value >> 4;
  value ^= value >> 2;
  value ^= value >> 1;
  return (value & 1) == 0;
}

static const char *condition_label(uint8_t condition) {
  switch (condition & 0x07) {
  case 0x00:
    return "NZ";
  case 0x01:
    return "Z";
  case 0x02:
    return "NC";
  case 0x03:
    return "C";
  case 0x04:
    return "PO";
  case 0x05:
    return "PE";
  case 0x06:
    return "P";
  case 0x07:
    return "M";
  default:
    return "?";
  }
}

static int condition_true(cpu_t *cpu, uint8_t condition) {
  uint8_t flags = (uint8_t)register_value_get(cpu, REG_F);
  switch (condition & 0x07) {
  case 0x00:
    return (flags & (1 << FLAG_Z)) == 0;
  case 0x01:
    return (flags & (1 << FLAG_Z)) != 0;
  case 0x02:
    return (flags & (1 << FLAG_C)) == 0;
  case 0x03:
    return (flags & (1 << FLAG_C)) != 0;
  case 0x04:
    return (flags & (1 << FLAG_PV)) == 0;
  case 0x05:
    return (flags & (1 << FLAG_PV)) != 0;
  case 0x06:
    return (flags & (1 << FLAG_S)) == 0;
  case 0x07:
    return (flags & (1 << FLAG_S)) != 0;
  default:
    return 0;
  }
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

static void update_flags_logic(cpu_t *cpu, uint8_t result, uint8_t half_carry) {
  flag_set(cpu, FLAG_S, result & 0x80);
  flag_set(cpu, FLAG_Z, result == 0);
  flag_set(cpu, FLAG_H, half_carry);
  flag_set(cpu, FLAG_PV, parity_even(result));
  register_flag_unset(cpu, FLAG_N);
  register_flag_unset(cpu, FLAG_C);
}

static void update_flags_rotate(cpu_t *cpu, uint8_t result, uint8_t carry) {
  flag_set(cpu, FLAG_S, result & 0x80);
  flag_set(cpu, FLAG_Z, result == 0);
  flag_set(cpu, FLAG_H, 0);
  flag_set(cpu, FLAG_PV, parity_even(result));
  register_flag_unset(cpu, FLAG_N);
  flag_set(cpu, FLAG_C, carry);
}

static uint8_t cb_read_value(cpu_t *cpu, uint8_t r_bits, uint8_t use_index,
                             uint8_t index_reg, uint8_t d,
                             uint16_t *address_out) {
  if (use_index || r_bits == 0x06) {
    uint16_t base = register_value_get(cpu, use_index ? index_reg : REG_HL);
    uint16_t address = (uint16_t)(base + (int8_t)d);
    if (address_out)
      *address_out = address;
    return memory_get(cpu, address);
  }

  if (address_out)
    *address_out = 0;
  return (uint8_t)register_value_get(cpu, register_map(r_bits));
}

static void cb_write_value(cpu_t *cpu, uint8_t r_bits, uint8_t use_index,
                           uint8_t index_reg, uint8_t d, uint16_t address,
                           uint8_t value) {
  if (use_index || r_bits == 0x06) {
    memory_set(cpu, address, value);
  }

  if (!use_index && r_bits != 0x06) {
    register_value_set(cpu, register_map(r_bits), value);
    return;
  }

  if (use_index && r_bits != 0x06) {
    register_value_set(cpu, register_map(r_bits), value);
  }
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

void inst_cb(cpu_t *cpu, uint8_t op_code, uint8_t use_index, uint8_t index_reg,
             uint8_t d) {
  uint8_t r_bits = op_code & 0x07;
  uint8_t group = (op_code >> 6) & 0x03;
  uint8_t op = (op_code >> 3) & 0x07;
  uint16_t address = 0;
  uint8_t value = 0;
  uint8_t result = 0;
  uint8_t carry = 0;
  const char *target = NULL;

  if (use_index || r_bits == 0x06) {
    target = use_index ? "(IX/IY+d)" : "(HL)";
  } else {
    target = register_name_8(register_map(r_bits));
  }

  value = cb_read_value(cpu, r_bits, use_index, index_reg, d, &address);

  if (group == 0) {
    switch (op) {
    case 0x00: // RLC
      carry = (value >> 7) & 1;
      result = (uint8_t)((value << 1) | carry);
      instruction_log(cpu, "RLC %s", target);
      update_flags_rotate(cpu, result, carry);
      cb_write_value(cpu, r_bits, use_index, index_reg, d, address, result);
      return;
    case 0x01: // RRC
      carry = value & 1;
      result = (uint8_t)((value >> 1) | (carry << 7));
      instruction_log(cpu, "RRC %s", target);
      update_flags_rotate(cpu, result, carry);
      cb_write_value(cpu, r_bits, use_index, index_reg, d, address, result);
      return;
    case 0x02: { // RL
      uint8_t carry_in =
          (register_value_get(cpu, REG_F) & (1 << FLAG_C)) ? 1 : 0;
      carry = (value >> 7) & 1;
      result = (uint8_t)((value << 1) | carry_in);
      instruction_log(cpu, "RL %s", target);
      update_flags_rotate(cpu, result, carry);
      cb_write_value(cpu, r_bits, use_index, index_reg, d, address, result);
      return;
    }
    case 0x03: { // RR
      uint8_t carry_in =
          (register_value_get(cpu, REG_F) & (1 << FLAG_C)) ? 1 : 0;
      carry = value & 1;
      result = (uint8_t)((value >> 1) | (carry_in << 7));
      instruction_log(cpu, "RR %s", target);
      update_flags_rotate(cpu, result, carry);
      cb_write_value(cpu, r_bits, use_index, index_reg, d, address, result);
      return;
    }
    case 0x04: // SLA
      carry = (value >> 7) & 1;
      result = (uint8_t)(value << 1);
      instruction_log(cpu, "SLA %s", target);
      update_flags_rotate(cpu, result, carry);
      cb_write_value(cpu, r_bits, use_index, index_reg, d, address, result);
      return;
    case 0x05: // SRA
      carry = value & 1;
      result = (uint8_t)((value >> 1) | (value & 0x80));
      instruction_log(cpu, "SRA %s", target);
      update_flags_rotate(cpu, result, carry);
      cb_write_value(cpu, r_bits, use_index, index_reg, d, address, result);
      return;
    case 0x06: // SLL (undocumented)
      carry = (value >> 7) & 1;
      result = (uint8_t)((value << 1) | 0x01);
      instruction_log(cpu, "SLL %s", target);
      update_flags_rotate(cpu, result, carry);
      cb_write_value(cpu, r_bits, use_index, index_reg, d, address, result);
      return;
    case 0x07: // SRL
      carry = value & 1;
      result = (uint8_t)(value >> 1);
      instruction_log(cpu, "SRL %s", target);
      update_flags_rotate(cpu, result, carry);
      cb_write_value(cpu, r_bits, use_index, index_reg, d, address, result);
      return;
    default:
      break;
    }
  } else if (group == 1) { // BIT
    uint8_t bit = op;
    uint8_t mask = (uint8_t)(1u << bit);
    uint8_t is_set = (value & mask) != 0;
    uint8_t carry_flag =
        (register_value_get(cpu, REG_F) & (1 << FLAG_C)) ? 1 : 0;

    instruction_log(cpu, "BIT %u,%s", bit, target);
    if (bit == 7)
      flag_set(cpu, FLAG_S, is_set);
    else
      flag_set(cpu, FLAG_S, 0);
    flag_set(cpu, FLAG_Z, !is_set);
    flag_set(cpu, FLAG_H, 1);
    flag_set(cpu, FLAG_PV, !is_set);
    register_flag_unset(cpu, FLAG_N);
    flag_set(cpu, FLAG_C, carry_flag);
    return;
  } else if (group == 2) { // RES
    uint8_t bit = op;
    result = (uint8_t)(value & ~(1u << bit));
    instruction_log(cpu, "RES %u,%s", bit, target);
    cb_write_value(cpu, r_bits, use_index, index_reg, d, address, result);
    return;
  } else if (group == 3) { // SET
    uint8_t bit = op;
    result = (uint8_t)(value | (1u << bit));
    instruction_log(cpu, "SET %u,%s", bit, target);
    cb_write_value(cpu, r_bits, use_index, index_reg, d, address, result);
    return;
  }

  instruction_log(cpu, "CB 0x%02X", op_code);
}

void inst_and_r(cpu_t *cpu, uint8_t op_code) {
  const char *label = NULL;
  uint8_t value = read_r_value(cpu, op_code, &label);
  uint8_t result = (uint8_t)register_value_get(cpu, REG_A) & value;

  instruction_log(cpu, "AND %s", label ? label : "?");
  register_value_set(cpu, REG_A, result);
  update_flags_logic(cpu, result, 1);
}

void inst_and_n(cpu_t *cpu, uint8_t value) {
  uint8_t result = (uint8_t)register_value_get(cpu, REG_A) & value;

  instruction_log(cpu, "AND 0x%02X", value);
  register_value_set(cpu, REG_A, result);
  update_flags_logic(cpu, result, 1);
}

void inst_and_idx(cpu_t *cpu, uint8_t index_reg, uint8_t d) {
  uint16_t address = register_value_get(cpu, index_reg);
  uint8_t value = memory_get(cpu, (uint16_t)(address + d));
  uint8_t result = (uint8_t)register_value_get(cpu, REG_A) & value;

  instruction_log(cpu, "AND (%s%+d)", register_name_16(index_reg),
                  (int8_t)d);
  register_value_set(cpu, REG_A, result);
  update_flags_logic(cpu, result, 1);
}

void inst_or_r(cpu_t *cpu, uint8_t op_code) {
  const char *label = NULL;
  uint8_t value = read_r_value(cpu, op_code, &label);
  uint8_t result = (uint8_t)register_value_get(cpu, REG_A) | value;

  instruction_log(cpu, "OR %s", label ? label : "?");
  register_value_set(cpu, REG_A, result);
  update_flags_logic(cpu, result, 0);
}

void inst_or_n(cpu_t *cpu, uint8_t value) {
  uint8_t result = (uint8_t)register_value_get(cpu, REG_A) | value;

  instruction_log(cpu, "OR 0x%02X", value);
  register_value_set(cpu, REG_A, result);
  update_flags_logic(cpu, result, 0);
}

void inst_or_idx(cpu_t *cpu, uint8_t index_reg, uint8_t d) {
  uint16_t address = register_value_get(cpu, index_reg);
  uint8_t value = memory_get(cpu, (uint16_t)(address + d));
  uint8_t result = (uint8_t)register_value_get(cpu, REG_A) | value;

  instruction_log(cpu, "OR (%s%+d)", register_name_16(index_reg),
                  (int8_t)d);
  register_value_set(cpu, REG_A, result);
  update_flags_logic(cpu, result, 0);
}

void inst_xor_r(cpu_t *cpu, uint8_t op_code) {
  const char *label = NULL;
  uint8_t value = read_r_value(cpu, op_code, &label);
  uint8_t result = (uint8_t)register_value_get(cpu, REG_A) ^ value;

  instruction_log(cpu, "XOR %s", label ? label : "?");
  register_value_set(cpu, REG_A, result);
  update_flags_logic(cpu, result, 0);
}

void inst_xor_n(cpu_t *cpu, uint8_t value) {
  uint8_t result = (uint8_t)register_value_get(cpu, REG_A) ^ value;

  instruction_log(cpu, "XOR 0x%02X", value);
  register_value_set(cpu, REG_A, result);
  update_flags_logic(cpu, result, 0);
}

void inst_xor_idx(cpu_t *cpu, uint8_t index_reg, uint8_t d) {
  uint16_t address = register_value_get(cpu, index_reg);
  uint8_t value = memory_get(cpu, (uint16_t)(address + d));
  uint8_t result = (uint8_t)register_value_get(cpu, REG_A) ^ value;

  instruction_log(cpu, "XOR (%s%+d)", register_name_16(index_reg),
                  (int8_t)d);
  register_value_set(cpu, REG_A, result);
  update_flags_logic(cpu, result, 0);
}

void inst_cp_r(cpu_t *cpu, uint8_t op_code) {
  const char *label = NULL;
  uint8_t value = read_r_value(cpu, op_code, &label);
  uint8_t a = (uint8_t)register_value_get(cpu, REG_A);
  int16_t diff = (int16_t)a - (int16_t)value;
  uint8_t result = (uint8_t)diff;

  instruction_log(cpu, "CP %s", label ? label : "?");
  update_flags_sub(cpu, a, value, 0, result, diff);
}

void inst_cp_n(cpu_t *cpu, uint8_t value) {
  uint8_t a = (uint8_t)register_value_get(cpu, REG_A);
  int16_t diff = (int16_t)a - (int16_t)value;
  uint8_t result = (uint8_t)diff;

  instruction_log(cpu, "CP 0x%02X", value);
  update_flags_sub(cpu, a, value, 0, result, diff);
}

void inst_cp_idx(cpu_t *cpu, uint8_t index_reg, uint8_t d) {
  uint16_t address = register_value_get(cpu, index_reg);
  uint8_t value = memory_get(cpu, (uint16_t)(address + d));
  uint8_t a = (uint8_t)register_value_get(cpu, REG_A);
  int16_t diff = (int16_t)a - (int16_t)value;
  uint8_t result = (uint8_t)diff;

  instruction_log(cpu, "CP (%s%+d)", register_name_16(index_reg),
                  (int8_t)d);
  update_flags_sub(cpu, a, value, 0, result, diff);
}

void inst_jr(cpu_t *cpu, uint8_t op_code, uint8_t displacement) {
  int8_t offset = (int8_t)displacement;
  uint16_t pc = register_value_get(cpu, REG_PC);
  int take = 0;

  if (op_code == 0x18) {
    instruction_log(cpu, "JR %+d", offset);
    take = 1;
  } else {
    uint8_t condition = (op_code >> 3) & 0x03;
    instruction_log(cpu, "JR %s,%+d", condition_label(condition), offset);
    take = condition_true(cpu, condition);
  }

  if (take) {
    register_value_set(cpu, REG_PC, (uint16_t)(pc + offset));
  }
}

void inst_jp(cpu_t *cpu, uint16_t op_code, uint16_t address) {
  if (op_code == 0xE9) {
    instruction_log(cpu, "JP (HL)");
    register_value_set(cpu, REG_PC, register_value_get(cpu, REG_HL));
    return;
  }
  if (op_code == 0xDDE9) {
    instruction_log(cpu, "JP (IX)");
    register_value_set(cpu, REG_PC, register_value_get(cpu, REG_IX));
    return;
  }
  if (op_code == 0xFDE9) {
    instruction_log(cpu, "JP (IY)");
    register_value_set(cpu, REG_PC, register_value_get(cpu, REG_IY));
    return;
  }

  if (op_code == 0xC3) {
    instruction_log(cpu, "JP 0x%04X", address);
    register_value_set(cpu, REG_PC, address);
    return;
  }

  if ((op_code & 0xC7) == 0xC2) {
    uint8_t condition = (op_code >> 3) & 0x07;
    instruction_log(cpu, "JP %s,0x%04X", condition_label(condition), address);
    if (condition_true(cpu, condition))
      register_value_set(cpu, REG_PC, address);
    return;
  }

  instruction_log(cpu, "JP 0x%04X", address);
  register_value_set(cpu, REG_PC, address);
}

void inst_call(cpu_t *cpu, uint16_t op_code, uint16_t address) {
  uint16_t pc = register_value_get(cpu, REG_PC);
  int take = 1;

  if (op_code != 0xCD) {
    uint8_t condition = (op_code >> 3) & 0x07;
    instruction_log(cpu, "CALL %s,0x%04X", condition_label(condition),
                    address);
    take = condition_true(cpu, condition);
  } else {
    instruction_log(cpu, "CALL 0x%04X", address);
  }

  if (!take)
    return;

  uint16_t sp = register_value_get(cpu, REG_SP);
  sp--;
  memory_set(cpu, sp, (uint8_t)((pc >> 8) & 0x00FF));
  sp--;
  memory_set(cpu, sp, (uint8_t)(pc & 0x00FF));
  register_value_set(cpu, REG_SP, sp);
  register_value_set(cpu, REG_PC, address);
}

void inst_ret(cpu_t *cpu, uint16_t op_code) {
  int take = 1;

  if (op_code == 0xC9) {
    instruction_log(cpu, "RET");
  } else if ((op_code & 0xC7) == 0xC0) {
    uint8_t condition = (op_code >> 3) & 0x07;
    instruction_log(cpu, "RET %s", condition_label(condition));
    take = condition_true(cpu, condition);
  } else if (op_code == 0xED4D || op_code == 0xED5D || op_code == 0xED6D ||
             op_code == 0xED7D) {
    instruction_log(cpu, "RETI");
  } else if (op_code == 0xED45 || op_code == 0xED55 || op_code == 0xED65 ||
             op_code == 0xED75) {
    instruction_log(cpu, "RETN");
  } else {
    instruction_log(cpu, "RET");
  }

  if (!take)
    return;

  uint16_t sp = register_value_get(cpu, REG_SP);
  uint8_t low = memory_get(cpu, sp);
  sp++;
  uint8_t high = memory_get(cpu, sp);
  sp++;
  register_value_set(cpu, REG_SP, sp);
  register_value_set(cpu, REG_PC, (uint16_t)((high << 8) | low));
}

void inst_rst(cpu_t *cpu, uint8_t op_code) {
  uint16_t pc = register_value_get(cpu, REG_PC);
  uint16_t sp = register_value_get(cpu, REG_SP);
  uint16_t vector = (uint16_t)(op_code & 0x38);

  instruction_log(cpu, "RST 0x%02X", (uint8_t)vector);
  sp--;
  memory_set(cpu, sp, (uint8_t)((pc >> 8) & 0x00FF));
  sp--;
  memory_set(cpu, sp, (uint8_t)(pc & 0x00FF));
  register_value_set(cpu, REG_SP, sp);
  register_value_set(cpu, REG_PC, vector);
}

void inst_daa(cpu_t *cpu) {
  uint8_t a = (uint8_t)register_value_get(cpu, REG_A);
  uint8_t flags = (uint8_t)register_value_get(cpu, REG_F);
  uint8_t adjust = 0;
  uint8_t carry = (flags & (1 << FLAG_C)) ? 1 : 0;
  uint8_t half = (flags & (1 << FLAG_H)) ? 1 : 0;
  uint8_t neg = (flags & (1 << FLAG_N)) ? 1 : 0;
  uint8_t result;

  if (!neg) {
    if (half || (a & 0x0F) > 0x09)
      adjust |= 0x06;
    if (carry || a > 0x99) {
      adjust |= 0x60;
      carry = 1;
    }
    result = (uint8_t)(a + adjust);
  } else {
    if (half)
      adjust |= 0x06;
    if (carry)
      adjust |= 0x60;
    result = (uint8_t)(a - adjust);
  }

  instruction_log(cpu, "DAA");
  register_value_set(cpu, REG_A, result);
  flag_set(cpu, FLAG_S, result & 0x80);
  flag_set(cpu, FLAG_Z, result == 0);
  flag_set(cpu, FLAG_H, ((a ^ result) & 0x10) != 0);
  flag_set(cpu, FLAG_PV, parity_even(result));
  flag_set(cpu, FLAG_C, carry);
}

void inst_cpl(cpu_t *cpu) {
  uint8_t a = (uint8_t)register_value_get(cpu, REG_A);
  uint8_t result = (uint8_t)~a;

  instruction_log(cpu, "CPL");
  register_value_set(cpu, REG_A, result);
  register_flag_set(cpu, FLAG_H);
  register_flag_set(cpu, FLAG_N);
}

void inst_neg(cpu_t *cpu) {
  uint8_t a = (uint8_t)register_value_get(cpu, REG_A);
  int16_t diff = 0 - (int16_t)a;
  uint8_t result = (uint8_t)diff;

  instruction_log(cpu, "NEG");
  register_value_set(cpu, REG_A, result);
  update_flags_sub(cpu, 0, a, 0, result, diff);
}

void inst_ccf(cpu_t *cpu) {
  uint8_t flags = (uint8_t)register_value_get(cpu, REG_F);
  uint8_t carry = (flags & (1 << FLAG_C)) ? 1 : 0;

  instruction_log(cpu, "CCF");
  flag_set(cpu, FLAG_C, !carry);
  flag_set(cpu, FLAG_H, carry);
  register_flag_unset(cpu, FLAG_N);
}

void inst_scf(cpu_t *cpu) {
  instruction_log(cpu, "SCF");
  register_flag_set(cpu, FLAG_C);
  register_flag_unset(cpu, FLAG_H);
  register_flag_unset(cpu, FLAG_N);
}

void inst_halt(cpu_t *cpu) {
  instruction_log(cpu, "HALT");
  cpu->halted = true;
}

void inst_di(cpu_t *cpu) {
  instruction_log(cpu, "DI");
  cpu->interrupts_enabled = false;
}

void inst_ei(cpu_t *cpu) {
  instruction_log(cpu, "EI");
  cpu->interrupts_enabled = true;
}

void inst_im(cpu_t *cpu, uint8_t mode) {
  instruction_log(cpu, "IM %u", mode);
  cpu->interrupt_mode = mode;
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
