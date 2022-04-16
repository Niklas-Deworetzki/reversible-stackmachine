
#include "assembler.h"

namespace Assembler {

    static void buildMemorySection(const Section &section, const SymbolTable &symbol_table,
                                   MemoryLayout &memory_layout) {
        iterate_section(section, [&](const Line &line) {
            switch (line.variant) {
                case LINE_WORDS: {
                    int32_t offset = 0;
                    for (OperandList *list = line.value.words.data; !list->isEmpty; list = list->tail) {
                        const int32_t address = line.base_address + offset;
                        memory_layout[address] = eval(list->head, address, symbol_table);

                        offset++;
                    }
                    break;
                }

                case LINE_SET:
                    if (line.value.setValue.memoryAddress.variant != PRIMITIVE_SYMBOL) {
                        memory_layout[line.base_address] = eval(line.value.setValue.value,
                                                                line.base_address, symbol_table);
                    }
                    break;

                default:
                    // Leave default initialized.
                    break;
            }
        });
    }

    void build_memory(const Program &program, const SymbolTable &symbol_table, MemoryLayout &memory_layout) {
        buildMemorySection(program.data, symbol_table, memory_layout);
        buildMemorySection(program.bss, symbol_table, memory_layout);
    }
}


