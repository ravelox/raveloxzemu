#include "cpu.h"

int cpu_init(cpu_t *cpu, uint32_t delay, uint16_t memory_size) {
  if (!cpu)
    return -1;

  cpu->last_instruction[0] = '\0';
  cpu->last_mem_read = 0;
  cpu->last_mem_write = 0;
  cpu->last_mem_read_valid = false;
  cpu->last_mem_write_valid = false;
  cpu->interrupts_enabled = false;
  cpu->halted = false;
  cpu->interrupt_mode = 0;

  if (register_init(cpu) != 0)
    return -1;
  if (clock_init(cpu, delay) != 0)
    return -1;
  if (memory_init(cpu, memory_size) != 0)
    return -1;

  return 0;
}

void cpu_destroy(cpu_t *cpu) {
  if (!cpu)
    return;

  memory_destroy(cpu);
  clock_destroy(cpu);
  register_destroy(cpu);
}
