#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clock.h"
#include "cpu.h"
#include "instruction.h"
#include "memory.h"
#include "register.h"
#include "test_program.h"

#define CLOCK_DELAY 0
#define MEMORY_SIZE (uint16_t)(64 * 1024) - 1

uint8_t get_byte_from_pc(cpu_t *cpu) {
  uint8_t value = memory_get(cpu, register_value_get(cpu, REG_PC));
  register_inc(cpu, REG_PC);
  return value;
}

uint16_t get_word_from_pc(cpu_t *cpu) {
  uint16_t value = 0;
  value = get_byte_from_pc(cpu);
  value = (value << 8) & get_byte_from_pc(cpu);
  return value;
}

int main(int argc, char *argv[]) {
  cpu_t *cpu = (cpu_t *)malloc(sizeof(cpu_t));

  if (!cpu) {
    fprintf(stderr, "Cannot allocate CPU\n");
    return -1;
  }

  if (cpu_init(cpu, CLOCK_DELAY, MEMORY_SIZE) != 0) {
    fprintf(stderr, "Cannot initialise CPU\n");
    free(cpu);
    return -1;
  }

  fprintf(stdout, "Memory size: %04x\n", memory_get_size(cpu));

  if (memory_load(cpu, test_program, test_program_size) != 0) {
    fprintf(stderr, "Cannot load test program\n");
    cpu_destroy(cpu);
    free(cpu);
    return -1;
  }

  // Set the stack pointer to the top of memory
  register_value_set(cpu, REG_SP, memory_get_size(cpu));

  while (1) {
    if (!clock_available(cpu))
      break;

    uint16_t value = 0;
    uint16_t op_code = 0;
    uint8_t idx = 0;
    uint8_t displacement = 0;
    uint16_t mem_addr = 0;
    uint8_t reg;

    op_code = get_byte_from_pc(cpu);

    // Special prefixes
    if (op_code == 0xDD || op_code == 0xFD || op_code == 0xED) {
      if (op_code == 0xDD) {
        idx = REG_IX;
      }
      if (op_code == 0xFD) {
        idx = REG_IY;
      }
      op_code = (op_code << 8) & get_byte_from_pc(cpu);
    }

    instruction_group_t group = instruction_group_get(op_code);

    switch (group) {
    case I_NOP:
      break;
    case I_LOAD_R_R:
      inst_load_r_r(cpu, (uint8_t)op_code);
      break;
    case I_LOAD_R_N:
      value = get_byte_from_pc(cpu);
      inst_load_r_n(cpu, (uint8_t)op_code, (uint8_t)value);
      break;
    case I_LOAD_R_HL:
      inst_load_r_hl(cpu, (uint8_t)op_code);
      break;
    case I_LOAD_R_IDX:
      displacement = get_byte_from_pc(cpu);
      inst_load_r_idx(cpu, op_code, idx, displacement);
      break;
    case I_LOAD_HL_R:
      inst_load_hl_r(cpu, (uint8_t)op_code);
      break;
    case I_LOAD_HL_N:
      value = get_byte_from_pc(cpu);
      inst_load_hl_n(cpu, (uint8_t)value);
      break;
    case I_LOAD_IDX_R:
      displacement = get_byte_from_pc(cpu);
      inst_load_idx_r(cpu, op_code, idx, displacement);
      break;
    case I_LOAD_A_MEM:
      mem_addr = get_word_from_pc(cpu);
      inst_load_a_mem(cpu, mem_addr);
      break;
    case I_LOAD_A_RR:
      if (op_code == 0x02) {
        reg = REG_BC;
      } else if (op_code == 0x12) {
        reg = REG_DE;
      } else {
        fprintf(stderr, "op_code: %02x\t Incorrect group\n", op_code);
        break;
      }
      inst_load_a_rr(cpu, reg);
      break;
    case I_LOAD_MEM_A:
      mem_addr = get_word_from_pc(cpu);
      inst_load_mem_a(cpu, mem_addr);
      break;
    case I_LOAD_RR_A:
      if (op_code == 0x0A) {
        reg = REG_BC;
      } else if (op_code == 0x1A) {
        reg = REG_DE;
      } else {
        fprintf(stderr, "op_code: %02x\t Incorrect group\n", op_code);
        break;
      }
      inst_load_rr_a(cpu, reg);
      break;
    case I_LOAD_RR_NN:
      mem_addr = get_word_from_pc(cpu);
      if (op_code == 0x01) {
        reg = REG_BC;
      } else if (op_code == 0x11) {
        reg = REG_DE;
      } else if (op_code == 0x21) {
        reg = REG_HL;
      } else if (op_code == 0x31) {
        reg = REG_SP;
      } else if (op_code == 0xDD21) {
        reg = REG_IX;
      } else if (op_code == 0xFD21) {
        reg = REG_IY;
      } else {
        fprintf(stderr, "op_code: %04x\t Incorrect group\n", op_code);
        break;
      }
      inst_load_rr_nn(cpu, reg, mem_addr);
      break;
    case I_LOAD_RR_MEM:
      mem_addr = get_word_from_pc(cpu);
      if (op_code == 0x2A) {
        reg = REG_HL;
      } else if (op_code == 0xDD2A) {
        reg = REG_IX;
      } else if (op_code == 0xFD2A) {
        reg = REG_IY;
      } else {
        fprintf(stderr, "op_code: %04x\t Incorrect group\n", op_code);
        break;
      }
      inst_load_rr_mem(cpu, reg, mem_addr);
      break;
    case I_LOAD_MEM_RR:
      mem_addr = get_word_from_pc(cpu);
      if (op_code == 0x22) {
        reg = REG_HL;
      } else if (op_code == 0xDD22) {
        reg = REG_IX;
      } else if (op_code == 0xFD22) {
        reg = REG_IY;
      } else {
        fprintf(stderr, "op_code: %04x\t Incorrect group\n", op_code);
        break;
      }
      inst_load_mem_rr(cpu, reg, mem_addr);
      break;
    case I_LOAD_SP_RR:
      if (op_code == 0xF9) {
        reg = REG_HL;
      } else if (op_code == 0xDDF9) {
        reg = REG_IX;
      } else if (op_code == 0xFDF9) {
        reg = REG_IY;
      } else {
        fprintf(stderr, "op_code: %04x\t Incorrect group\n", op_code);
        break;
      }
      inst_load_sp_rr(cpu, reg);
      break;
    case I_LOAD_SP_MEM:
      mem_addr = get_word_from_pc(cpu);
      inst_load_sp_mem(cpu, mem_addr);
      break;
    case I_LOAD_MEM_SP:
      mem_addr = get_word_from_pc(cpu);
      inst_load_mem_sp(cpu, mem_addr);
      break;
    case I_ADD_A_R:
      inst_add_a_r(cpu, (uint8_t)op_code);
      break;
    case I_ADD_A_N:
      value = get_byte_from_pc(cpu);
      inst_add_a_n(cpu, (uint8_t)value);
      break;
    case I_ADD_A_IDX:
      displacement = get_byte_from_pc(cpu);
      inst_add_a_idx(cpu, idx, displacement);
      break;
    case I_ADC_A_R:
      inst_adc_a_r(cpu, (uint8_t)op_code);
      break;
    case I_ADC_A_N:
      value = get_byte_from_pc(cpu);
      inst_adc_a_n(cpu, (uint8_t)value);
      break;
    case I_ADC_A_IDX:
      displacement = get_byte_from_pc(cpu);
      inst_adc_a_idx(cpu, idx, displacement);
      break;
    case I_SUB_R:
      inst_sub_r(cpu, (uint8_t)op_code);
      break;
    case I_SUB_N:
      value = get_byte_from_pc(cpu);
      inst_sub_n(cpu, (uint8_t)value);
      break;
    case I_SUB_IDX:
      displacement = get_byte_from_pc(cpu);
      inst_sub_idx(cpu, idx, displacement);
      break;
    case I_SBC_A_R:
      inst_sbc_a_r(cpu, (uint8_t)op_code);
      break;
    case I_SBC_A_N:
      value = get_byte_from_pc(cpu);
      inst_sbc_a_n(cpu, (uint8_t)value);
      break;
    case I_SBC_A_IDX:
      displacement = get_byte_from_pc(cpu);
      inst_sbc_a_idx(cpu, idx, displacement);
      break;
    case I_INC_R:
      inst_inc_r(cpu, (uint8_t)op_code);
      break;
    case I_INC_HL:
      inst_inc_hl(cpu);
      break;
    case I_INC_IDX:
      displacement = get_byte_from_pc(cpu);
      inst_inc_idx(cpu, idx, displacement);
      break;
    case I_DEC_R:
      inst_dec_r(cpu, (uint8_t)op_code);
      break;
    case I_DEC_HL:
      inst_dec_hl(cpu);
      break;
    case I_DEC_IDX:
      displacement = get_byte_from_pc(cpu);
      inst_dec_idx(cpu, idx, displacement);
      break;
    case I_PUSH:
      if (op_code == 0xC5) {
        reg = REG_BC;
      } else if (op_code == 0xD5) {
        reg = REG_DE;
      } else if (op_code == 0xE5) {
        reg = REG_HL;
      } else if (op_code == 0xF5) {
        reg = REG_AF;
      } else if (op_code == 0xDDE5) {
        reg = REG_IX;
      } else if (op_code == 0xFDE5) {
        reg = REG_IY;
      } else {
        fprintf(stderr, "op_code: %04x\t Incorrect group\n", op_code);
        break;
      }
      inst_push_rr(cpu, reg);
      break;
    case I_POP:
      if (op_code == 0xC1) {
        reg = REG_BC;
      } else if (op_code == 0xD1) {
        reg = REG_DE;
      } else if (op_code == 0xE1) {
        reg = REG_HL;
      } else if (op_code == 0xF1) {
        reg = REG_AF;
      } else if (op_code == 0xDDE1) {
        reg = REG_IX;
      } else if (op_code == 0xFDE1) {
        reg = REG_IY;
      } else {
        fprintf(stderr, "op_code: %04x\t Incorrect group\n", op_code);
        break;
      }
      inst_pop_rr(cpu, reg);
      break;
    case I_LOAD_A_I:
      _load_r_r(cpu, REG_I, REG_A);
      break;
    case I_LOAD_A_R_REG:
      _load_r_r(cpu, REG_R, REG_A);
      break;
    case I_LOAD_I_A:
      _load_r_r(cpu, REG_A, REG_I);
      break;
    case I_LOAD_R_REG_A:
      _load_r_r(cpu, REG_R, REG_A);
      break;
    case I_LOAD_IDX_N:
      displacement = get_byte_from_pc(cpu);
      value = get_byte_from_pc(cpu);
      inst_load_idx_n(cpu, idx, displacement, value);
      break;
    case I_BLKT:
      inst_blkt(cpu, op_code);
      break;
    case I_BLKS:
      inst_blks(cpu, op_code);
      break;
    case I_EX:
      switch (op_code) {
      case 0x08:
        register_ex_af_af_alt(cpu);
        break;
      case 0xE3:
        register_ex_sp_hl(cpu);
        break;
      case 0xEB:
        register_ex_de_hl(cpu);
        break;
      case 0xDDE3:
        register_ex_sp_ix(cpu);
        break;
      case 0xFDE3:
        register_ex_sp_iy(cpu);
        break;
      default:
        fprintf(stderr, "op_code: %04x\t Incorrect EX group\n", op_code);
        break;
      }
      break;
    default:
      fprintf(stderr, "Undefined opcode: %04x\t%u\n", op_code, group);
      break;
    }

    register_display(cpu);
    if (clock_delay(cpu) == -1)
      break;
  }

  cpu_destroy(cpu);
  free(cpu);

  return 0;
}
