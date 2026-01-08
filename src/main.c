#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clock.h"
#include "cpu.h"
#include "instruction.h"
#include "memory.h"
#include "register.h"
#include "test_program.h"

#define CLOCK_DELAY 1000
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

static void dump_memory_window(cpu_t *cpu, uint16_t address) {
  uint16_t base = (uint16_t)(address & 0xFFF0);

  for (uint16_t row = 0; row < 2; row++) {
    uint16_t row_addr = (uint16_t)(base + row * 16);
    fprintf(stdout, "  %04X  ", row_addr);
    for (uint16_t col = 0; col < 16; col++) {
      uint16_t addr = (uint16_t)(row_addr + col);
      fprintf(stdout, "%02X ", (uint8_t)cpu->memory.memory[addr]);
    }
    fprintf(stdout, " |");
    for (uint16_t col = 0; col < 16; col++) {
      uint16_t addr = (uint16_t)(row_addr + col);
      uint8_t value = (uint8_t)cpu->memory.memory[addr];
      fputc(isprint(value) ? value : '.', stdout);
    }
    fprintf(stdout, "|\n");
  }
}

static void strip_enclosing_quotes(char *value) {
  size_t len = 0;

  if (!value)
    return;

  if (value[0] == '\\' && (value[1] == '"' || value[1] == '\'')) {
    memmove(value, value + 1, strlen(value));
  }

  len = strlen(value);
  if (len < 2)
    return;

  if ((value[0] == '"' && value[len - 1] == '"' &&
       (len < 2 || value[len - 2] != '\\')) ||
      (value[0] == '\'' && value[len - 1] == '\'' &&
       (len < 2 || value[len - 2] != '\\'))) {
    value[len - 1] = '\0';
    memmove(value, value + 1, len - 1);
  }
}

static int load_file_to_memory(cpu_t *cpu, const char *path,
                               uint16_t address) {
  FILE *file = fopen(path, "rb");
  size_t bytes_read = 0;

  if (!file) {
    fprintf(stderr, "Cannot open file: %s\n", path);
    return -1;
  }

  bytes_read = fread(cpu->memory.memory + address, 1,
                     cpu->memory.size - address, file);
  fclose(file);

  if (bytes_read == 0) {
    fprintf(stderr, "No data read from file: %s\n", path);
    return -1;
  }

  fprintf(stdout, "Loaded %zu bytes at %04X\n", bytes_read, address);
  return 0;
}

static int dump_memory_to_file(cpu_t *cpu, const char *path, uint16_t address,
                               size_t length) {
  FILE *file = fopen(path, "wb");
  size_t bytes_written = 0;
  size_t max_len = cpu->memory.size - address;

  if (!file) {
    fprintf(stderr, "Cannot open file: %s\n", path);
    return -1;
  }

  if (length > max_len)
    length = max_len;

  bytes_written = fwrite(cpu->memory.memory + address, 1, length, file);
  fclose(file);

  if (bytes_written != length) {
    fprintf(stderr, "Failed to write file: %s\n", path);
    return -1;
  }

  fprintf(stdout, "Wrote %zu bytes from %04X to %s\n", bytes_written, address,
          path);
  return 0;
}

static int parse_hex(const char *text, uint16_t *value_out) {
  char *end = NULL;
  unsigned long value = strtoul(text, &end, 16);

  if (!text || text == end)
    return -1;

  while (*end != '\0') {
    if (!isspace((unsigned char)*end))
      return -1;
    end++;
  }

  if (value > 0xFFFF)
    return -1;

  *value_out = (uint16_t)value;
  return 0;
}

static int execute_instruction(cpu_t *cpu) {
  uint16_t value = 0;
  uint16_t op_code = 0;
  uint8_t idx = 0;
  uint8_t displacement = 0;
  uint16_t mem_addr = 0;
  uint8_t reg;

  if (!clock_available(cpu))
    return -1;

  op_code = get_byte_from_pc(cpu);

  // Special prefixes
  if (op_code == 0xCB) {
    uint8_t cb_op = get_byte_from_pc(cpu);
    inst_cb(cpu, cb_op, 0, REG_HL, 0);
    goto instruction_done;
  }

  if (op_code == 0xDD || op_code == 0xFD) {
    uint8_t prefix = (uint8_t)op_code;
    uint8_t next = get_byte_from_pc(cpu);

    idx = (prefix == 0xDD) ? REG_IX : REG_IY;
    if (next == 0xCB) {
      displacement = get_byte_from_pc(cpu);
      uint8_t cb_op = get_byte_from_pc(cpu);
      inst_cb(cpu, cb_op, 1, idx, displacement);
      goto instruction_done;
    }

    op_code = (uint16_t)((prefix << 8) | next);
  } else if (op_code == 0xED) {
    uint8_t next = get_byte_from_pc(cpu);
    op_code = (uint16_t)((op_code << 8) | next);
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
    case I_AND_R:
      inst_and_r(cpu, (uint8_t)op_code);
      break;
    case I_AND_N:
      value = get_byte_from_pc(cpu);
      inst_and_n(cpu, (uint8_t)value);
      break;
    case I_AND_IDX:
      displacement = get_byte_from_pc(cpu);
      inst_and_idx(cpu, idx, displacement);
      break;
    case I_OR_R:
      inst_or_r(cpu, (uint8_t)op_code);
      break;
    case I_OR_N:
      value = get_byte_from_pc(cpu);
      inst_or_n(cpu, (uint8_t)value);
      break;
    case I_OR_IDX:
      displacement = get_byte_from_pc(cpu);
      inst_or_idx(cpu, idx, displacement);
      break;
    case I_XOR_R:
      inst_xor_r(cpu, (uint8_t)op_code);
      break;
    case I_XOR_N:
      value = get_byte_from_pc(cpu);
      inst_xor_n(cpu, (uint8_t)value);
      break;
    case I_XOR_IDX:
      displacement = get_byte_from_pc(cpu);
      inst_xor_idx(cpu, idx, displacement);
      break;
    case I_CP_R:
      inst_cp_r(cpu, (uint8_t)op_code);
      break;
    case I_CP_N:
      value = get_byte_from_pc(cpu);
      inst_cp_n(cpu, (uint8_t)value);
      break;
    case I_CP_IDX:
      displacement = get_byte_from_pc(cpu);
      inst_cp_idx(cpu, idx, displacement);
      break;
    case I_JR:
      displacement = get_byte_from_pc(cpu);
      inst_jr(cpu, (uint8_t)op_code, displacement);
      break;
    case I_JP:
      if (op_code == 0xE9 || op_code == 0xDDE9 || op_code == 0xFDE9) {
        inst_jp(cpu, op_code, 0);
      } else {
        mem_addr = get_word_from_pc(cpu);
        inst_jp(cpu, op_code, mem_addr);
      }
      break;
    case I_CALL:
      mem_addr = get_word_from_pc(cpu);
      inst_call(cpu, op_code, mem_addr);
      break;
    case I_RET:
      inst_ret(cpu, op_code);
      break;
    case I_RST:
      inst_rst(cpu, (uint8_t)op_code);
      break;
    case I_DAA:
      inst_daa(cpu);
      break;
    case I_CPL:
      inst_cpl(cpu);
      break;
    case I_NEG:
      inst_neg(cpu);
      break;
    case I_CCF:
      inst_ccf(cpu);
      break;
    case I_SCF:
      inst_scf(cpu);
      break;
    case I_HALT:
      inst_halt(cpu);
      break;
    case I_DI:
      inst_di(cpu);
      break;
    case I_EI:
      inst_ei(cpu);
      break;
    case I_IM:
      if (op_code == 0xED46 || op_code == 0xED4E || op_code == 0xED66 ||
          op_code == 0xED6E) {
        inst_im(cpu, 0);
      } else if (op_code == 0xED56 || op_code == 0xED76) {
        inst_im(cpu, 1);
      } else if (op_code == 0xED5E || op_code == 0xED7E) {
        inst_im(cpu, 2);
      } else {
        fprintf(stderr, "op_code: %04x\t Incorrect group\n", op_code);
      }
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

instruction_done:
  if (cpu->halted)
    return 1;

  register_display(cpu);
  if (clock_delay(cpu) == -1)
    return -1;

  return 0;
}

static int run_from_address(cpu_t *cpu, uint16_t address) {
  cpu->halted = false;
  register_value_set(cpu, REG_PC, address);
  register_value_set(cpu, REG_SP, memory_get_size(cpu));

  while (1) {
    int status = execute_instruction(cpu);
    if (status != 0)
      return status;
  }

  return 0;
}

static int run_until_halt(cpu_t *cpu) {
  cpu->halted = false;
  while (1) {
    int status = execute_instruction(cpu);
    if (status != 0)
      return status;
  }

  return 0;
}

static void debugger_prompt(cpu_t *cpu) {
  char line[128];
  int has_run = 0;

  while (1) {
    fprintf(stdout, "\n(debug) ");
    if (!fgets(line, sizeof(line), stdin))
      break;

    char *cmd = line;
    while (isspace((unsigned char)*cmd))
      cmd++;

    if (*cmd == '\0') {
      if (cpu->clock.delay == 0 && has_run) {
        cpu->halted = false;
        execute_instruction(cpu);
      }
      continue;
    }

    char *arg = cmd;
    while (*arg && !isspace((unsigned char)*arg))
      arg++;
    if (*arg) {
      *arg = '\0';
      arg++;
    }

    while (*arg && isspace((unsigned char)*arg))
      arg++;

    if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "q") == 0) {
      break;
    }

    if (strcmp(cmd, "run") == 0 || strcmp(cmd, "r") == 0) {
      uint16_t address = 0;
      if (*arg == '\0') {
        address = (uint16_t)register_value_get(cpu, REG_PC);
      } else if (parse_hex(arg, &address) != 0) {
        fprintf(stdout, "Usage: run [hex_address]\n");
        continue;
      }
      run_from_address(cpu, address);
      has_run = 1;
      continue;
    }

    if ((strcmp(cmd, "next") == 0 || strcmp(cmd, "n") == 0) &&
        cpu->clock.delay == 0) {
      if (has_run) {
        cpu->halted = false;
        execute_instruction(cpu);
      }
      continue;
    }

    if ((strcmp(cmd, "cont") == 0 || strcmp(cmd, "c") == 0) &&
        cpu->clock.delay == 0) {
      if (has_run) {
        run_until_halt(cpu);
      }
      continue;
    }

    if (strcmp(cmd, "mem") == 0 || strcmp(cmd, "m") == 0) {
      uint16_t address = 0;
      if (*arg == '\0') {
        address = (uint16_t)register_value_get(cpu, REG_PC);
      } else if (parse_hex(arg, &address) != 0) {
        fprintf(stdout, "Usage: mem [hex_address]\n");
        continue;
      }
      dump_memory_window(cpu, address);
      continue;
    }

    if (strcmp(cmd, "delay") == 0 || strcmp(cmd, "d") == 0) {
      if (*arg == '\0') {
        fprintf(stdout, "Clock delay: %u\n", cpu->clock.delay);
        continue;
      }
      char *end = NULL;
      unsigned long value = strtoul(arg, &end, 10);
      if (arg == end) {
        fprintf(stdout, "Usage: delay <value>\n");
        continue;
      }
      cpu->clock.delay = (uint32_t)value;
      fprintf(stdout, "Clock delay set to %u\n", cpu->clock.delay);
      continue;
    }

    if (strcmp(cmd, "load") == 0 || strcmp(cmd, "l") == 0) {
      char *path = arg;
      while (*arg && !isspace((unsigned char)*arg))
        arg++;
      if (*arg) {
        *arg = '\0';
        arg++;
      }
      while (*arg && isspace((unsigned char)*arg))
        arg++;

      if (*path == '\0' || *arg == '\0') {
        fprintf(stdout, "Usage: load <path> <hex_address>\n");
        continue;
      }

      strip_enclosing_quotes(path);

      uint16_t address = 0;
      if (parse_hex(arg, &address) != 0) {
        fprintf(stdout, "Usage: load <path> <hex_address>\n");
        continue;
      }

      load_file_to_memory(cpu, path, address);
      continue;
    }

    if (strcmp(cmd, "dump") == 0 || strcmp(cmd, "x") == 0) {
      char *path = arg;
      while (*arg && !isspace((unsigned char)*arg))
        arg++;
      if (*arg) {
        *arg = '\0';
        arg++;
      }
      while (*arg && isspace((unsigned char)*arg))
        arg++;

      char *addr_str = arg;
      while (*arg && !isspace((unsigned char)*arg))
        arg++;
      if (*arg) {
        *arg = '\0';
        arg++;
      }
      while (*arg && isspace((unsigned char)*arg))
        arg++;

      char *len_str = arg;

      if (*path == '\0' || *addr_str == '\0' || *len_str == '\0') {
        fprintf(stdout, "Usage: dump <path> <hex_address> <length>\n");
        continue;
      }

      strip_enclosing_quotes(path);

      uint16_t address = 0;
      if (parse_hex(addr_str, &address) != 0) {
        fprintf(stdout, "Usage: dump <path> <hex_address> <length>\n");
        continue;
      }

      char *end = NULL;
      unsigned long length = strtoul(len_str, &end, 10);
      if (len_str == end) {
        fprintf(stdout, "Usage: dump <path> <hex_address> <length>\n");
        continue;
      }

      dump_memory_to_file(cpu, path, address, (size_t)length);
      continue;
    }

    if (strcmp(cmd, "help") == 0 || strcmp(cmd, "h") == 0 ||
        strcmp(cmd, "usage") == 0) {
      fprintf(stdout,
              "Commands:\n"
              "  run [hex]    start execution (defaults to PC)\n"
              "  mem [hex]    show 32-byte memory window (defaults to PC)\n"
              "  delay [n]    show/set clock delay\n"
              "  load <path> <hex>   load file at address\n"
              "  dump <path> <hex> <len>  dump memory to file\n"
              "  next         step one instruction (delay=0)\n"
              "  cont         run until HALT (delay=0)\n"
              "  quit         exit emulator\n");
      continue;
    }

    fprintf(stdout,
            "Commands: run [hex], mem [hex], delay [value], load <path> <hex>, "
            "dump <path> <hex> <len>, next, cont, help, quit\n");
  }
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

  debugger_prompt(cpu);

  cpu_destroy(cpu);
  free(cpu);

  return 0;
}
