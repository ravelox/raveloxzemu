#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint16_t op_code;
    char *label;
    uint8_t extra_bytes;
    uint8_t op_type;
} z80_instruction_t;

