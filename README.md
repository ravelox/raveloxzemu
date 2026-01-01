# raveloxzemu

A small C-based Z80 emulator experiment. Currently includes register definitions and a minimal executable scaffold.

## Build

```sh
cmake -S . -B build
cmake --build build
```

The resulting binary is placed in `build/`.

## Project layout

- `src/` — source files for the emulator.
- `include/` — public headers.
- `CMakeLists.txt` — CMake build configuration.
