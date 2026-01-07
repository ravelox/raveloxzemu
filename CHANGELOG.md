# Changelog

All notable changes to this project will be documented in this file.

## [0.1.0] - 2024-01-01
- Initialize CMake-based build.
- Add basic register definitions and empty main scaffold.
- Add git metadata and ignore rules.

## Unreleased

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
