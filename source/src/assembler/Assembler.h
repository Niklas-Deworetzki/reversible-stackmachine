#pragma once

#include <map>
#include <string>
#include <optional>
#include "syntax/Syntax.h"
#include "messages/error.h"

namespace Assembler {

    using SymbolTable = std::map<std::string, int32_t>;
    using MemoryLayout = std::map<int32_t, int32_t>;

    /**
     * Evaluate an Operand that is either a constant or combines only constants.
     */
    [[nodiscard]] int32_t restrict_eval(const Operand &operand);

    /**
     * Evaluate an Operand.
     *
     * @param operand The evaluated operand.
     * @param position The position where this operand occurs.
     *                 This is used to evaluate relative operands.
     * @param symbol_table A symbol table used to fetch the value of symbolic operands.
     */
    [[nodiscard]] int32_t eval(const Operand &operand, int32_t position, const SymbolTable &symbol_table);


    /**
     * Constructs a SymbolTable from the given program. This will reserve memory areas in the
     * MemoryLayout and create entries for every defined symbol in the SymbolTable.
     *
     * @param program The Program defining symbols and elements to layout in memory.
     * @param memory_layout The memory layout in which areas are reserved.
     * @param base_address The base address used to layout memory areas.
     * @return A new SymbolTable holding values for every defined symbol.
     */
    [[nodiscard]] SymbolTable resolve_symbols(Program &program, MemoryLayout &memory_layout, int32_t base_address);

    /**
     * Fills the reserved memory areas with their associated values.
     *
     * @param program The program defining values for different memory areas.
     * @param symbol_table The SymbolTable used to resolve the values of symbolic operands in memory.
     * @param memory_layout The MemoryLayout updated with values.
     */
    void build_memory(const Program &program, const SymbolTable &symbol_table, MemoryLayout &memory_layout);

    /**
     * Translates all Instructions in a program to their corresponding bit-patterns, resolving
     * instruction opcodes and operand values.
     *
     * @param program The program defining Instructions.
     * @param symbol_table The SymbolTable used to resolve the values of symbolic operands.
     * @return
     */
    [[nodiscard]] std::tuple<std::vector<int32_t>, int32_t> translate_program(
            const Program &program, const SymbolTable &symbol_table);

    /**
     * Assembles the given Program, resolving symbols, building a MemoryLayout and translating
     * defined instructions into their corresponding bit-patterns.
     *
     * @param program The Program to assemble.
     * @return The MemoryLayout defined for the program, a vector holding translated bit-patterns
     * of instructions and the programs entry point.
     */
    [[nodiscard]] std::tuple<MemoryLayout, std::vector<int32_t>, int32_t> assemble(Program &program);

}
