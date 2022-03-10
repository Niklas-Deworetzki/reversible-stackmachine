
#include <iostream>
#include "Assembler.h"

namespace Assembler {

    using std::cerr, std::endl;

    [[nodiscard]] static int32_t lookup_entry(const SymbolTable &table, std::optional<std::string> &entry_point) {
        if (entry_point->empty()) {
            return 0;
        }
        if (!table.contains(entry_point.value())) {
            cerr << "[WARNING] Entry point '" + entry_point.value() + "' is not defined! Starting at offset 0 instead."
                 << endl;
            return 0;
        }
        return table.at(entry_point.value());
    }

    [[nodiscard]] std::tuple<MemoryLayout, std::vector<int32_t>, int32_t> assemble(
            Program &program, std::optional<std::string> &entry_point) {
        MemoryLayout memory;
        const SymbolTable table = resolve_symbols(program, memory, 0);
        build_memory(program, table, memory);
        return {memory, translate_program(program, table), lookup_entry(table, entry_point)};
    }
}
