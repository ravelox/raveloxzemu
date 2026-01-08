# raveloxzemu

A small C-based Z80 emulator experiment. Currently includes register definitions/helpers, a simple clock, memory allocation, an instruction table stub, and a minimal executable that prints the register state on each step.

## Build

```sh
cmake -S . -B build
cmake --build build
```

The resulting binary is placed in `build/`.

`CMAKE_EXPORT_COMPILE_COMMANDS` is enabled, so `compile_commands.json` is emitted at the project root for tooling.

## Run

```sh
./build/raveloxzemu
```

The binary initializes the registers, clock (delay loop), and 64KB of memory, then loads the built-in test program. Use the interactive prompt to run code and inspect memory.

Debugger commands:
- `run <hex_address>` — start execution from a memory address (resets `SP`).
- `mem <hex_address>` — display a 32-byte memory window.
- `delay [value]` — show or set the clock delay (0 disables delay).
- `load <path> <hex_address>` — load a file into memory at an address.
- `dump <path> <hex_address> <length>` — save memory to a file.
- `quit` — exit the emulator.

## Register helpers

- `register_init`/`register_destroy` allocate and clean up register banks; call them before and after use.
- `register_display` prints general and special registers for both active and alternate banks, including Z80 flags (S, Z, H, PV, N, C) via a byte/word-accessible API.
- `register_value_set`/`register_value_get` handle 8-bit and 16-bit registers using the provided `REG_*` constants; `register_inc`/`register_dec` adjust registers and `register_swap` swaps the primary/alternate banks.
- Flag helpers (`register_flag_set`, `register_flag_unset`, `register_bit_*`) operate on `F`.

## Clock

- `clock_init`/`clock_destroy` create and clean up the simple emulator clock.
- `clock_delay` respects an artificial delay; when set to 0, execution waits for Enter to simulate single-step behavior.

## Memory

- `memory_init`/`memory_destroy` allocate and free a contiguous memory block.
- `memory_get_size` returns the configured memory size; used to seed the stack pointer.
- `memory_set`/`memory_get` read/write bytes at addresses; `memory_load` allows bulk loading.

## Instructions

- `instruction.c` maps opcodes to instruction groups with human-readable labels, implements a starter `LD` instruction set (including IX/IY indexed, I/R transfer variants, and `LD (HL), n`), includes block transfer/search helpers, and covers EX + PUSH/POP + 8-bit arithmetic.
- `instruction.h` defines instruction groups and the helper APIs used by the opcode dispatcher.
- Raw opcode listings live in `src/op_codes.txt`.

## Project layout

- `src/` — source files for the emulator.
- `include/` — public headers.
- `CMakeLists.txt` — CMake build configuration.
- `src/test_program.c` / `include/test_program.h` — built-in sample program loaded at startup.
