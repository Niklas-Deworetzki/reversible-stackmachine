
#include <cstring>
#include "instructions.h"

static InstructionData *INVERSE_DATA = nullptr;
static constexpr size_t INSTRUCTION_COUNT = sizeof(KNOWN_INSTRUCTIONS) / sizeof(*KNOWN_INSTRUCTIONS);


[[nodiscard]] static InstructionData inverse(const InstructionData &instruction) noexcept {
    InstructionData result = {
            .fw_mnemonic = instruction.bw_mnemonic,
            .bw_mnemonic = instruction.fw_mnemonic,
            .operand_mode = instruction.operand_mode,
            .binary = INVERSE(instruction.binary),
    };
    return result;
}

[[nodiscard]] static InstructionData *build_inverse_data() noexcept {
    static InstructionData data[INSTRUCTION_COUNT];

    for (size_t i = 0; i < INSTRUCTION_COUNT; i++) {
        data[i] = inverse(KNOWN_INSTRUCTIONS[i]);
    }

    return data;
}

[[nodiscard]] const InstructionData &InstructionData::get(size_t offset, bool is_forward) noexcept {
    if (INVERSE_DATA == nullptr) {
        INVERSE_DATA = build_inverse_data();
    }

    return ((is_forward) ? KNOWN_INSTRUCTIONS : INVERSE_DATA)[offset];
}