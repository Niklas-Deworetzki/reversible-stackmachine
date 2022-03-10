
#include <cstring>
#include <iostream>
#include "Assembler.h"
#include "syntax/Instructions.h"

namespace Assembler {
    using std::cerr, std::endl;

    [[nodiscard]] std::vector<int32_t> translate_program(const Program &program, const SymbolTable &symbol_table) {
        std::vector<int32_t> result;
        bool contains_halt = false;

        iterate_section(program.code, [&](const Line &line) {
            const Instruction &instruction = line.value.instruction;
            contains_halt = contains_halt || (strcmp(instruction.data->fw_mnemonic, "halt") == 0);

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
                    }
                    break;

                default:
                    operand = operand_low_value(operand);
                    break;
            }

            result.push_back(opcode | operand);
        });

        if (!contains_halt) {
            cerr << "[WARNING] Program does not contain a 'halt' instruction. It can only exit abnormally." << endl;
        }
        return result;
    }
}