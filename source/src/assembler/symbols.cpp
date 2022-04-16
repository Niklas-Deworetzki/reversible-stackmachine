
#include <stdexcept>
#include "assembler.h"
#include "messages/error.h"

namespace Assembler {

    constexpr int32_t DEFAULT_MEMORY_VALUE = 0;

    [[nodiscard]] static int32_t allocate_range(MemoryLayout &memory_layout, int32_t &base_address, size_t size) {
        const auto castedSize = static_cast<int32_t>(size);
        if (castedSize < 0 || (base_address + castedSize) < 0)
            throw std::out_of_range("Requested memory cannot be allocated with current layout.");

        // Check if range collides with any allocated address in memory layout.
        for (int32_t address = base_address + castedSize - 1; address >= base_address; address--) {
            if (memory_layout.contains(address)) {
                base_address = address + 1;
                return allocate_range(memory_layout, base_address, size);
            }
        }

        // Reserve memory range in layout.
        for (int32_t address = base_address; address < base_address + castedSize; address++) {
            memory_layout[address] = DEFAULT_MEMORY_VALUE;
        }

        // Move base_address for next allocation.
        int32_t result = base_address;
        base_address += castedSize;

        return result;
    }

    static void enter_symbol(SymbolTable &symbol_table, const char *symbol, int32_t value) {
        if (symbol_table.contains(symbol)) {
            throw symbol_redefinition_error(symbol);
        }

        symbol_table[symbol] = value;
    }

    static void enter_symbols(SymbolTable &symbol_table, Line &line, int32_t address) {
        line.base_address = address;

        for (StringList *labels = line.labels; !labels->isEmpty; labels = labels->tail) {
            enter_symbol(symbol_table, labels->head, address);
        }
    }


    static void layout_fixed(MemoryLayout &memory_layout, SymbolTable &symbol_table, Section &section) {
        iterate_section(section, [&](Line &line) {
            if (line.variant == LINE_SET && line.value.setValue.memoryAddress.variant != PRIMITIVE_SYMBOL) {
                const int32_t address = restrict_eval(line.value.setValue.memoryAddress);

                if (memory_layout.contains(address)) {
                    throw set_address_clash(address);
                }

                memory_layout[address] = DEFAULT_MEMORY_VALUE;
                enter_symbols(symbol_table, line, address);
            } else if (line.variant == LINE_SET) {
                const int32_t value = restrict_eval(line.value.setValue.value);

                enter_symbol(symbol_table, line.value.setValue.memoryAddress.instance.primitive.value.symbol, value);
                enter_symbols(symbol_table, line, value);
            }
        });
    }

    static void layout_section(MemoryLayout &memory_layout, SymbolTable &symbol_table,
                               Section &section, int32_t &base_address,
                               const LineVariant allowed_mask) {
        iterate_section(section, [&](Line &line) {
            if ((line.variant & allowed_mask) != line.variant) {
                throw illegal_section_content();
            }

            switch (line.variant) {
                case LINE_INSTRUCTION:
                    enter_symbols(symbol_table, line, base_address);
                    base_address += 1;
                    break;

                case LINE_RESERVED: {
                    const int32_t size = restrict_eval(line.value.reserved.size);
                    const int32_t allocatedAddress = allocate_range(memory_layout, base_address, size);
                    enter_symbols(symbol_table, line, allocatedAddress);
                    break;
                }

                case LINE_WORDS: {
                    int32_t size = 0;
                    for (OperandList *list = line.value.words.data; !list->isEmpty; list = list->tail) {
                        size++;
                    }

                    const int32_t allocatedAddress = allocate_range(memory_layout, base_address, size);
                    enter_symbols(symbol_table, line, allocatedAddress);
                    break;
                }

                case LINE_SET:
                    break; // Nothing to do here.
            }
        });
    }

    [[nodiscard]] SymbolTable resolve_symbols(Program &program, MemoryLayout &memory_layout, int32_t base_address) {
        SymbolTable symbol_table;

        layout_fixed(memory_layout, symbol_table, program.data);
        layout_section(memory_layout, symbol_table, program.data, base_address,
                       static_cast<LineVariant>(LINE_WORDS | LINE_SET));
        layout_section(memory_layout, symbol_table, program.bss, base_address,
                       LINE_RESERVED);

        int instruction_count = 0;
        layout_section(memory_layout, symbol_table, program.code, instruction_count, LINE_INSTRUCTION);

        return symbol_table;
    }
}

