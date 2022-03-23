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


    [[nodiscard]] SymbolTable resolve_symbols(Program &program, MemoryLayout &memory_layout, int32_t base_address);

    void build_memory(const Program &program, const SymbolTable &symbol_table, MemoryLayout &memoryLayout);

    [[nodiscard]] std::tuple<std::vector<int32_t>, int32_t> translate_program(
            const Program &program, const SymbolTable &symbol_table);

    [[nodiscard]] std::tuple<MemoryLayout, std::vector<int32_t>, int32_t> assemble(Program &program);

}
