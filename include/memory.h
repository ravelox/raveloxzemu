#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint16_t size;
  char *memory;
} z80_memory_t;

int memory_init(uint16_t);
int memory_destroy(void);

uint16_t memory_get_size(void);

int memory_load(uint8_t, size_t);

int memory_set(uint16_t, uint8_t);
uint8_t memory_get(uint16_t);
#endif