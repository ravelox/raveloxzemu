#ifndef OPCODE_TABLE_H
#define OPCODE_TABLE_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint8_t bytes[4];
  uint8_t length;
  uint8_t has_displacement;
  const char *label;
} opcode_info_t;

extern const opcode_info_t opcode_table[];
extern const size_t opcode_table_size;

#endif
