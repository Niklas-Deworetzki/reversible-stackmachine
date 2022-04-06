#pragma once

#include <cstdint>
#include <optional>

/**
 * Amount of bits within an instruction available for the instruction opcode.
 */
constexpr size_t OPCODE_WIDTH = 16;
/**
 * Amount of bits within an instruction available for the instruction operand.
 */
constexpr size_t OPERAND_WIDTH = 32 - OPCODE_WIDTH;

// We want to be able to load a word with at most two instructions.
static_assert(OPCODE_WIDTH <= OPERAND_WIDTH);

/*
 * Returns an integer with the lowest n bits set.
 */
[[nodiscard]] static constexpr int32_t setNBits(int32_t n) noexcept {
    int32_t result = 0;
    for (int i = 0; i < n; ++i) {
        result = 1 | (result << 1);
    }
    return result;
}

/**
 * A mask to extract all bits of an operand from a word.
 * The operand must be in the lowest part of the word.
 */
constexpr int32_t OPERAND_WIDTH_MASK = setNBits(OPERAND_WIDTH);
/**
 * A mask to extract all bits except its sign from a word.
 * The operand must be in the lowest part of the word.
 */
constexpr int32_t SIGNLESS_OPERAND_WIDTH_MASK = setNBits(OPERAND_WIDTH - 1);

/**
 * A mask to extract all bits of an opcode from a word.
 * The opcode must be in the lowest part of the word.
 */
constexpr int32_t OPCODE_WIDTH_MASK = setNBits(OPCODE_WIDTH);

/**
 * A mask to extract the sign of an operand from a word.
 * The operand must be in the lowest part of the word.
 */
constexpr int32_t OPERAND_SIGN = 1 << (OPERAND_WIDTH - 1);
/**
 * A mask with all bits set, that are also set in a negative number
 * but are outside of the width of an operand.
 *
 * It can be used to extend the sign of an operand, from the operands
 * width to a full word.
 */
constexpr int32_t SIGN_EXTEND_MASK = ~OPCODE_WIDTH_MASK;

/**
 * Extracts the lower bits of a word as an operand.
 */
[[nodiscard]] static constexpr int32_t operand_low_value(int32_t operand) noexcept {
    return (operand & SIGNLESS_OPERAND_WIDTH_MASK) | (OPERAND_SIGN & (operand >> OPCODE_WIDTH));
}

/**
 * Extracts the higher bits of a word as an operand.
 */
[[nodiscard]] static constexpr int32_t operand_high_value(int32_t operand) noexcept {
    int32_t higher_bits = OPERAND_WIDTH_MASK & (operand >> (OPERAND_WIDTH - 1));
    if (operand < 0) higher_bits = OPCODE_WIDTH_MASK & ~higher_bits; // invert, since it is xor-ed on the sign extension.
    return higher_bits;
}

/**
 * Extends an operand to a word, keeping the sign.
 */
[[nodiscard]] static constexpr int32_t sign_extend(int32_t operand) noexcept {
    return (operand & OPERAND_SIGN) ? SIGN_EXTEND_MASK | operand
                                    : operand;
}

/**
 * The bit used to distinguish forwards and backwards variants of
 * an instruction pair.
 */
constexpr int32_t DIRECTION_BIT = 1 << (OPCODE_WIDTH - 1);
/**
 * Inverts an opcode to the backwards variant of this instruction.
 * The opcode must be in the lowest part of the given word.
 */
#define INVERSE(opcode) ((opcode) ^ DIRECTION_BIT)

enum class OperandMode {
    /**
     * Instruction expects no operand.
     */
    NO_OPERAND,
    /**
     * The absolute value of an operand is used for an instruction.
     */
    ABSOLUTE,
    /**
     * The relative offset between operand and instruction is used.
     */
    RELATIVE,
    /**
     * The upper bits of an operand are used for an instruction.
     */
    UPPER
};

struct InstructionData {
    const char *fw_mnemonic;
    const char *bw_mnemonic;
    OperandMode operand_mode;
    int32_t binary;

    [[nodiscard]] static const InstructionData &get(size_t offset, bool is_forward) noexcept;
};

constexpr InstructionData KNOWN_INSTRUCTIONS[] = {
        {"start",     "stop",       OperandMode::NO_OPERAND, 0},
        {"nop",       "nop",        OperandMode::NO_OPERAND, 1},

        {"pushc",     "popc",       OperandMode::ABSOLUTE,   2},

        {"dup",       "undup",      OperandMode::NO_OPERAND, 3},
        {"swap",      "swap",       OperandMode::NO_OPERAND, 4},
        {"bury",      "dig",        OperandMode::NO_OPERAND, 5},

        {"allocpar",  "releasepar", OperandMode::ABSOLUTE,   6},

        {"asf",       "rsf",        OperandMode::ABSOLUTE,   7},
        {"pushl",     "popl",       OperandMode::ABSOLUTE,   8},

        {"call",      "call",       OperandMode::NO_OPERAND, 9},
        {"uncall",    "uncall",     OperandMode::NO_OPERAND, 10},
        {"branch",    "branch",     OperandMode::RELATIVE,   11},
        {"brt",       "brt",        OperandMode::RELATIVE,   12},
        {"brf",       "brf",        OperandMode::RELATIVE,   13},

        {"pushtrue",  "poptrue",    OperandMode::NO_OPERAND, 14},
        {"pushfalse", "popfalse",   OperandMode::NO_OPERAND, 15},

        {"cmpusheq",  "cmpopeq",    OperandMode::NO_OPERAND, 16},
        {"cmpushne",  "cmpopne",    OperandMode::NO_OPERAND, 17},
        {"cmpushlt",  "cmpoplt",    OperandMode::NO_OPERAND, 18},
        {"cmpushle",  "cmpople",    OperandMode::NO_OPERAND, 19},

        {"inc",       "dec",        OperandMode::ABSOLUTE,   20},
        {"neg",       "neg",        OperandMode::NO_OPERAND, 21},

        {"add",       "sub",        OperandMode::NO_OPERAND, 22},
        {"xor",       "xor",        OperandMode::NO_OPERAND, 23},
        {"shl",       "shr",        OperandMode::NO_OPERAND, 24},

        {"arpushadd", "arpopadd",   OperandMode::NO_OPERAND, 25},
        {"arpushsub", "arpopsub",   OperandMode::NO_OPERAND, 26},
        {"arpushmul", "arpopmul",   OperandMode::NO_OPERAND, 27},
        {"arpushdiv", "arpopdiv",   OperandMode::NO_OPERAND, 28},
        {"arpushmod", "arpopmod",   OperandMode::NO_OPERAND, 29},
        {"arpushand", "arpopand",   OperandMode::NO_OPERAND, 30},
        {"arpushor",  "arpopor",    OperandMode::NO_OPERAND, 31},

        {"pushm",     "popm",       OperandMode::ABSOLUTE,   32},
        {"load",      "store",      OperandMode::ABSOLUTE,   33},
        {"memswap",   "memswap",    OperandMode::NO_OPERAND, 34},

        {"xorhc",     "xorhc",      OperandMode::UPPER,      35},
};

// Ensure opcode width is large enough to represent all opcodes.
static_assert((1 << (OPCODE_WIDTH - 1)) >= sizeof(KNOWN_INSTRUCTIONS) / sizeof(*KNOWN_INSTRUCTIONS));
