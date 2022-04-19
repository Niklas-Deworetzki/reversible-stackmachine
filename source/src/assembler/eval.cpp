/**
 * Implement evaluation of operands.
 */

#include "assembler.h"
#include "messages/error.h"

namespace Assembler {

    [[nodiscard]] static int32_t restrict_eval(const PrimitiveOperand &operand) {
        if (operand.variant == PRIMITIVE_CONSTANT) {
            return operand.value.constant;
        }
        // Symbols and relative operands cannot be evaluated here.
        throw invalid_operand("Restricted evaluation not possible.");
    }

    [[nodiscard]] int32_t restrict_eval(const Operand &operand) {
        switch (operand.variant) {
            case PRIMITIVE_SYMBOL:
            case PRIMITIVE_CONSTANT:
            case PRIMITIVE_RELATIVE:
                return restrict_eval(operand.instance.primitive);

            case COMPLEX_ADD:
                return restrict_eval(operand.instance.complex.lhs) + restrict_eval(operand.instance.complex.rhs);
            case COMPLEX_SUB:
                return restrict_eval(operand.instance.complex.lhs) - restrict_eval(operand.instance.complex.rhs);
            default:
                throw invalid_operand("Not a valid operand.");
        }
    }


    [[nodiscard]] static int32_t eval(const PrimitiveOperand &operand, const int32_t position, const SymbolTable &symbol_table) {
        switch (operand.variant) {
            case PRIMITIVE_SYMBOL: {
                std::string symbol = operand.value.symbol;
                if (symbol_table.contains(symbol)) {
                    return symbol_table.at(symbol);
                } else {
                    throw invalid_operand("Unknown symbol.");
                }
            }

            case PRIMITIVE_CONSTANT:
                return operand.value.constant;

            case PRIMITIVE_RELATIVE:
                return position + operand.value.relative;

            default:
                throw invalid_operand("Not a valid operand.");
        }

    }

    [[nodiscard]] int32_t eval(const Operand &operand, const int32_t position, const SymbolTable &symbol_table) {
        switch (operand.variant) {
            case PRIMITIVE_SYMBOL:
            case PRIMITIVE_CONSTANT:
            case PRIMITIVE_RELATIVE:
                return eval(operand.instance.primitive, position, symbol_table);
            case NO_OPERAND:
                return 0;
            case COMPLEX_ADD:
                return eval(operand.instance.complex.lhs, position, symbol_table) +
                       eval(operand.instance.complex.rhs, position, symbol_table);
            case COMPLEX_SUB:
                return eval(operand.instance.complex.lhs, position, symbol_table) -
                       eval(operand.instance.complex.rhs, position, symbol_table);
            default:
                throw invalid_operand("Not a valid operand.");
        }
    }
}
