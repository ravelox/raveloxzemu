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

int memory_load(uint8_t *buffer, size_t size);

#endif