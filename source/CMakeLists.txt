cmake_minimum_required(VERSION 3.21)
project(stackmachine)

OPTION(UNSAFE_OPERATIONS "Instructs the machine to skip some runtime checks in order to improve performance." OFF)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-Wall -Wextra)
add_compile_options(-Wno-implicit-fallthrough)
add_compile_options(-g)
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -march=native -flto)
endif ()

IF(UNSAFE_OPERATIONS)
    add_definitions(-DUNSAFE_OPERATIONS)
ENDIF()

include_directories(src)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/generated-sources)

FIND_PACKAGE(BISON REQUIRED)
FIND_PACKAGE(FLEX REQUIRED)


bison_target(PARSER
        ${CMAKE_CURRENT_SOURCE_DIR}/src/syntax/parser.y
        ${CMAKE_CURRENT_BINARY_DIR}/generated-sources/syntax/parser.c
        # Relative operands introduce shift/reduce conflict, but we want to favor shifting.
        COMPILE_FLAGS -Wno-conflicts-sr)

flex_target(SCANNER
        ${CMAKE_CURRENT_SOURCE_DIR}/src/syntax/scanner.flex
        ${CMAKE_CURRENT_BINARY_DIR}/generated-sources/syntax/scanner.c)

add_executable(scannergen
        src/syntax/Syntax.h
        src/scannergen/main.cpp)

add_executable(stackmachine
        ${BISON_PARSER_OUTPUTS}
        ${FLEX_SCANNER_OUTPUTS}
        main.cpp
        src/machine/Machine.cpp
        src/assembler/Assembler.cpp src/assembler/Eval.cpp src/assembler/Memory.cpp src/assembler/Symbols.cpp src/assembler/Translation.cpp
        src/syntax/csyntax.c src/syntax/Syntax.cpp src/syntax/Instructions.cpp
        )


