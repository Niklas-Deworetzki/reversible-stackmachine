
#include <cstring>
#include <iostream>
#include "Assembler.h"
#include "syntax/Instructions.h"

namespace Assembler {
    using std::cerr, std::endl;

    [[nodiscard]] std::tuple<std::vector<int32_t>, int32_t> translate_program(
            const Program &program, const SymbolTable &symbol_table) {
        std::vector<int32_t> result;

        bool contains_start = false, contains_stop = false;
        int32_t entry_address = 0;

        iterate_section(program.code, [&](const Line &line) {
            const Instruction &instruction = line.value.instruction;

            const char *mnemonic = instruction.is_forward
                                   ? instruction.data->fw_mnemonic
                                   : instruction.data->bw_mnemonic;
            if (strcmp(mnemonic, "start") == 0) {
                if (contains_start) {
                    throw start_stop_presence("start");
                }

                contains_start = true;
                entry_address = line.base_address;

            } else if (strcmp(mnemonic, "stop") == 0) {
                if (contains_stop) {
                    throw start_stop_presence("stop");
                }

                contains_stop = true;
            }

            const int32_t opcode = instruction.data->binary << OPERAND_WIDTH;
            int32_t operand = eval(instruction.operand, line.base_address, symbol_table);
            switch (instruction.data->operand_mode) {
                case OperandMode::RELATIVE:
                    operand = operand_low_value(operand - line.base_address);
                    break;

                case OperandMode::UPPER:
                    operand = operand_high_value(operand);
                    break;

                case OperandMode::NO_OPERAND:
                    if (operand != 0) {
                        fprintf(stderr,
                                "[WARNING] Line %d: Operand is discarded. Instruction %s does not accept an operand, but %d is provided.\n",
                                line.linenumber, instruction.data->fw_mnemonic, operand);
                        // Enforce discarding of operands. Keeping the value would change the instructions bit-pattern.
                        operand = 0;
                    }
                    break;

                default:
                    operand = operand_low_value(operand);
                    break;
            }

            result.push_back(opcode | operand);
        });

        return {result, entry_address};
    }
}