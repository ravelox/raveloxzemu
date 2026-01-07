#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clock.h"
#include "instruction.h"
#include "memory.h"
#include "register.h"

#define CLOCK_DELAY 0
#define MEMORY_SIZE (uint16_t)(64 * 1024) - 1

uint8_t get_byte_from_pc(void) {
  uint8_t value = memory_get(register_value_get(REG_PC));
  register_inc(REG_PC);
  return value;
}

uint16_t get_word_from_pc(void) {
  uint16_t value = 0;
  value = get_byte_from_pc();
  value = (value << 8) & get_byte_from_pc();
  return value;
}

int main(int argc, char *argv[]) {
  if (register_init() != 0) {
    fprintf(stderr, "Cannot initialise registers\n");
    return -1;
  };

  if (clock_init(CLOCK_DELAY) != 0) {
    fprintf(stderr, "Cannot initialise clock\n");
    return -1;
  }

  if (memory_init(MEMORY_SIZE) != 0) {
    fprintf(stderr, "Cannot intialize memory\n");
    return -1;
  }

  fprintf(stdout, "Memory size: %04x\n", memory_get_size());

  // Set the stack pointer to the top of memory
  register_value_set(REG_SP, memory_get_size());

  while (1) {
    if (!clock_available())
      break;

    uint16_t value = 0;
    uint16_t op_code = 0;
    uint8_t idx = 0;
    uint8_t displacement = 0;
    uint16_t mem_addr = 0;
    uint8_t reg;

    op_code = get_byte_from_pc();

    // Special prefixes
    if (op_code == 0xdd || op_code == 0xfd || op_code == 0xed) {
      if (op_code == 0xdd) {
        idx = REG_IX;
      }
      if (op_code == 0xfd) {
        idx = REG_IY;
      }
      op_code = (op_code << 8) & get_byte_from_pc();
    }

    instruction_group_t group = instruction_group_get(op_code);

    switch (group) {
    case I_NOP:
      break;
    case I_LOAD_R_R:
      inst_load_r_r((uint8_t)(op_code & 0x00FF));
      break;
    case I_LOAD_R_N:
      value = get_byte_from_pc();
      inst_load_r_n((uint8_t)(op_code & 0x00FF), (uint8_t)(value & 0x00FF));
      break;
    case I_LOAD_R_HL:
      inst_load_r_hl((uint8_t)(op_code & 0x00FF));
      break;
    case I_LOAD_R_IDX:
      displacement = get_byte_from_pc();
      inst_load_r_idx(op_code, idx, displacement);
      break;
    case I_LOAD_HL_R:
      inst_load_hl_r((uint8_t)(op_code & 0x00FF));
      break;
    case I_LOAD_IDX_R:
      displacement = get_byte_from_pc();
      inst_load_idx_r(op_code, idx, displacement);
      break;
    case I_LOAD_A_MEM:
      mem_addr = get_word_from_pc();
      inst_load_a_mem(mem_addr);
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
      inst_load_a_rr(reg);
      break;
    case I_LOAD_MEM_A:
      mem_addr = get_word_from_pc();
      inst_load_mem_a(mem_addr);
      break;
    case I_LOAD_RR_A:
      if (op_code == 0x0a) {
        reg = REG_BC;
      } else if (op_code == 0x1a) {
        reg = REG_DE;
      } else {
        fprintf(stderr, "op_code: %02x\t Incorrect group\n", op_code);
        break;
      }
      inst_load_rr_a(reg);
      break;
    case I_LOAD_A_I:
      _load_r_r(REG_I, REG_A);
      break;
    case I_LOAD_A_R_REG:
      _load_r_r(REG_R, REG_A);
      break;
    case I_LOAD_I_A:
      _load_r_r(REG_A, REG_I);
      break;
    case I_LOAD_R_REG_A:
      _load_r_r(REG_R, REG_A);
      break;
    default:
      fprintf(stderr, "Undefined opcode: %04x\t%u\n", op_code, group);
      break;
    }

    register_display();
    if (clock_delay() == -1)
      break;
  }

  clock_destroy();
  register_destroy();

  return 0;
}