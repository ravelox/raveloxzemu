# Changelog

All notable changes to this project will be documented in this file.

## [0.1.0] - 2024-01-01
- Initialize CMake-based build.
- Add basic register definitions and empty main scaffold.
- Add git metadata and ignore rules.

## [0.2.0] - 2026-01-02
- Expand register helpers to use a unified byte/word API, add getters/setters, increment/decrement helpers, flag bit utilities, and alternate bank swapping.
- Enable `CMAKE_EXPORT_COMPILE_COMMANDS` and document build/run workflow.
- Add simple clock module with optional step-through delay for single-stepping.
- Add memory allocation module with byte get/set and bulk load helpers; seed stack pointer to top of memory.
- Introduce instruction module stub with starter opcode table sourced from `src/op_codes.txt`.

## Unreleased
