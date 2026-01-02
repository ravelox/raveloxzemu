# Changelog

All notable changes to this project will be documented in this file.

## [0.1.0] - 2024-01-01
- Initialize CMake-based build.
- Add basic register definitions and empty main scaffold.
- Add git metadata and ignore rules.

## Unreleased
- Add register helper implementations with init, destroy, display, swap, and setters.
- Enable `CMAKE_EXPORT_COMPILE_COMMANDS` and document build/run workflow.
- Add getter helpers for general and special registers plus program-counter increment.
- Introduce simple clock module with optional step-through delay.
- Add memory allocation module and initialize 64KB memory with stack pointer seeded to the top.
