# raveloxzemu

A small C-based Z80 emulator experiment with:
- CPU instance model (no globals for registers/memory/clock).
- Debugger-style prompt for run/step/memory inspection and load/dump.
- ANSI register + memory display with last instruction logging.
- Growing instruction set: LD/EX/PUSH/POP, 8-bit arithmetic + logic, 16-bit arithmetic, control flow, CB-prefixed ops.
- Built-in test program covering supported instructions.

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
- `run [hex_address]` — start execution from a memory address (defaults to `PC`, resets `SP`).
- `mem [hex_address]` — display a 32-byte memory window (defaults to `PC`).
- `set <hex_address> <hex_byte...>` — write one or more bytes starting at the address.
- `delay [value]` — show or set the clock delay (0 enables step/cont).
- `load <path> <hex_address>` — load a file into memory at an address.
- `dump <path> <hex_address> <length>` — save memory to a file.
- `next` — execute one instruction (delay must be 0).
- `cont` — run until HALT (delay must be 0).
- `help` — display available commands.
- `quit` — exit the emulator.

## Register helpers

- `register_init`/`register_destroy` allocate and clean up register banks; call them before and after use.
- `register_display` prints general and special registers for both active and alternate banks, including Z80 flags (S, Z, H, PV, N, C) via a byte/word-accessible API.
- `register_value_set`/`register_value_get` handle 8-bit and 16-bit registers using the provided `REG_*` constants; `register_inc`/`register_dec` adjust registers and `register_swap` swaps the primary/alternate banks.
- Flag helpers (`register_flag_set`, `register_flag_unset`, `register_bit_*`) operate on `F`.

## Clock

- `clock_init`/`clock_destroy` create and clean up the simple emulator clock.
- `clock_delay` respects an artificial delay; when set to 0, stepping/continuing is controlled via the debugger prompt.

## Memory

- `memory_init`/`memory_destroy` allocate and free a contiguous memory block.
- `memory_get_size` returns the configured memory size; used to seed the stack pointer.
- `memory_set`/`memory_get` read/write bytes at addresses; `memory_load` allows bulk loading.

## Instructions

- `instruction.c` maps opcodes to instruction groups with human-readable labels, implements `LD` (including IX/IY indexed, I/R transfer variants), EX + PUSH/POP, 8-bit arithmetic/logical ops, control flow (JR/JP/CALL/RET/RST), block transfer/search helpers, and CB-prefixed rotate/shift/bit/set/res behavior.
- `instruction.h` defines instruction groups and the helper APIs used by the opcode dispatcher.
- Raw opcode listings live in `src/op_codes.txt`.
- `opcode_table.*` is a generated opcode table used for reference/labeling.

## Project layout

- `src/` — source files for the emulator.
- `include/` — public headers.
- `CMakeLists.txt` — CMake build configuration.
- `src/test_program.c` / `include/test_program.h` — built-in sample program loaded at startup.
