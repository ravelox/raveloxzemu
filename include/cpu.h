#ifndef CPU_H
#define CPU_H

#include <stdint.h>

#include "clock.h"
#include "memory.h"
#include "register.h"

struct cpu {
  z80_clock_t clock;
  z80_memory_t memory;
  z80_register_t registers[REG_COUNT];
  z80_register_t alt_registers[REG_COUNT];
};

int cpu_init(cpu_t *cpu, uint32_t delay, uint16_t memory_size);
void cpu_destroy(cpu_t *cpu);

#endif
