
#include <iostream>
#include "Assembler.h"

namespace Assembler {

    [[nodiscard]] std::tuple<MemoryLayout, std::vector<int32_t>, int32_t> assemble(
            Program &program) {
        MemoryLayout memory;

        const SymbolTable table = resolve_symbols(program, memory, 0);
        build_memory(program, table, memory);
        const auto &[code, entry_address] = translate_program(program, table);
        return {memory, code, entry_address};
    }
}
