# raveloxzemu

A small C-based Z80 emulator experiment. Currently includes register definitions, register helpers, and a minimal executable that prints the register state.

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

The binary initializes the register banks, dumps both the active and alternate register sets (including flags), and then exits.

## Register helpers

- `register_init`/`register_destroy` allocate and clean up register banks; call them before and after use.
- `register_display` prints general and special registers for both active and alternate banks, including the Z80 flag bits (S, Z, H, PV, N, C).
- Setter helpers exist for individual 8-bit and 16-bit registers, plus swap functions for toggling between the primary and alternate banks.

## Project layout

- `src/` — source files for the emulator.
- `include/` — public headers.
- `CMakeLists.txt` — CMake build configuration.
