#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"

static z80_memory_t *mem;

int memory_init(uint16_t memory_size) {

  mem = (z80_memory_t *)malloc(sizeof(z80_memory_t));
  if (!mem) {
    fprintf(stderr, "Cannot allocate internal memory structure\n");
    return -1;
  }

  mem->size = memory_size;
  mem->memory = (char *)malloc(mem->size);

  if (!mem->memory) {
    fprintf(stderr, "Cannot allocate memory space: %u\n", mem->size);
    return -1;
  }

  memset(mem->memory, 0, mem->size);

  fprintf(stdout, "Memory allocated: %x\n", memory_size);
  return 0;
}

int memory_destroy(void) {
  if (!mem)
    return 0;
  if (mem->memory)
    free(mem->memory);
  free(mem);
  return 0;
}

uint16_t memory_get_size(void) {
    if( ! mem ) {
        fprintf(stderr, "Memory not allocated\n");
        return 0;
    }
    return mem->size;
}