cmake_minimum_required(VERSION 3.16)

project(raveloxzemu VERSION 0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
    src/main.c
    src/cpu.c
    src/register.c
    src/clock.c
    src/memory.c
    src/instruction.c
    src/test_program.c
)

add_executable(raveloxzemu ${SOURCES})

target_include_directories(raveloxzemu PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
