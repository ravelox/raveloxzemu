# Changelog

All notable changes to this project will be documented in this file.

## [0.1.0] - 2024-01-01
- Initialize CMake-based build.
- Add basic register definitions and empty main scaffold.
- Add git metadata and ignore rules.

## Unreleased

## [0.4.7] - 2026-01-07
- Add control instruction helpers (DAA/CPL/NEG/CCF/SCF/DI/EI/IM/HALT).
- Move execution control to a debugger-style prompt with load/dump commands.
- Track debugger file-path quoting and add HALT to the test program.

## [0.4.6] - 2026-01-07
- Add 8-bit arithmetic helpers (ADD/ADC/SUB/SBC/INC/DEC) and dispatch logic.
- Track and render CPU state with ANSI output, current instruction, and memory windows.
- Expand the built-in test program with arithmetic cases.

## [0.4.5] - 2026-01-07
- Refactor emulator state into a `cpu_t` instance passed through all helpers.
- Add CPU init/destroy helpers and forward declarations to simplify includes.
- Expand the built-in test program and keep it compiled into the emulator.

## [0.4.4] - 2026-01-07
- Add EX helpers for register and stack exchanges.
- Map EX opcodes and dispatch them in the instruction switch.

## [0.4.3] - 2026-01-06
- Add opcode labels to the instruction map for easier identification.
- Dispatch block transfer/search instructions and correct flag/repeat behavior.
- Ignore the local `raveloxzemu` binary.

## [0.4.2] - 2026-01-06
- Extend the opcode map to cover the remaining `LD` register and memory variants.
- Add `LD (HL), n` to the opcode map and dispatch it in the main loop.

## [0.4.1] - 2026-01-06
- Expand the opcode map to cover more `LD` variants, including immediate, indexed, and I/R transfer forms.
- Add indexed-immediate `LD` execution in the main opcode dispatcher.
- Ignore `blap` helper files in the repo.

## [0.4.0] - 2026-01-06
- Replace the instruction table stub with grouped opcode mapping and `LD` instruction helpers.
- Decode prefixed opcodes and execute the supported `LD` instructions in the main loop.

## [0.2.0] - 2026-01-02
- Expand register helpers to use a unified byte/word API, add getters/setters, increment/decrement helpers, flag bit utilities, and alternate bank swapping.
- Enable `CMAKE_EXPORT_COMPILE_COMMANDS` and document build/run workflow.
- Add simple clock module with optional step-through delay for single-stepping.
- Add memory allocation module with byte get/set and bulk load helpers; seed stack pointer to top of memory.
- Introduce instruction module stub with starter opcode table sourced from `src/op_codes.txt`.
