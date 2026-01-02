# raveloxzemu

A small C-based Z80 emulator experiment. Currently includes register definitions, register helpers, a simple clock, memory allocation, and a minimal executable that prints the register state on each step.

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

The binary initializes the registers, clock (step mode by default), and 64KB of memory, then repeatedly dumps the register state while incrementing `PC`. With the default `CLOCK_DELAY` of 0 you advance each step by pressing Enter.

## Register helpers

- `register_init`/`register_destroy` allocate and clean up register banks; call them before and after use.
- `register_display` prints general and special registers for both active and alternate banks, including the Z80 flag bits (S, Z, H, PV, N, C).
- Setter and getter helpers exist for individual 8-bit and 16-bit registers, plus swap functions for toggling between the primary and alternate banks.
- `register_inc_pc` moves the program counter forward by one; stack pointer helpers (`register_set_sp`, `register_get_sp`) let you target the top of memory for the stack.

## Clock

- `clock_init`/`clock_destroy` create and clean up the simple emulator clock.
- `clock_delay` respects an artificial delay; when set to 0, execution waits for Enter to simulate single-step behavior.

## Memory

- `memory_init`/`memory_destroy` allocate and free a contiguous memory block.
- `memory_get_size` returns the configured memory size; used to seed the stack pointer.

## Project layout

- `src/` — source files for the emulator.
- `include/` — public headers.
- `CMakeLists.txt` — CMake build configuration.
