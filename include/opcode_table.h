/*
 * This file is part of raveloxzemu.
 *
 * Copyright (C) 2026 Dave Kelly
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
