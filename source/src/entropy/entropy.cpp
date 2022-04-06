
#include <bit>
#include <cinttypes>
#include <iostream>
#include "assembler/Assembler.h"
#include "entropy.h"
#include "machine/Machine.h"

namespace Entropy {
    template<unsigned int (entropy_generated_with_words)(int32_t, int32_t)>
    static unsigned long long count_entropy(const Assembler::MemoryLayout &original_memory,
                                            const Machine::VM &machine) {
        unsigned long long result = 0;
        for (int32_t address = 0; (unsigned) address < machine.memory.size(); address++) {
            if (original_memory.contains(address)) {
                result += entropy_generated_with_words(machine.memory[address], original_memory.at(address));
            } else {
                result += entropy_generated_with_words(machine.memory[address], 0);
            }
        }
        for (int32_t stack_address = 0; stack_address < machine.sp; stack_address++) {
            result += entropy_generated_with_words(machine.stack[stack_address], 0);
        }
        result += entropy_generated_with_words(machine.sp, 0);
        return result;
    }

    static unsigned int hamming_weight(int32_t a, int32_t b) {
        int32_t difference_bits = a ^ b;
        return std::popcount(*reinterpret_cast<uint32_t *>(&difference_bits));
    }

    static unsigned int word_difference(int32_t a, int32_t b) {
        return (a != b) ? 1 : 0;
    }


    void report_entropy(Measure measure,
                        const Assembler::MemoryLayout &original_memory,
                        const Machine::VM &machine) {
        if (measure == Measure::NONE) return;

        std::cerr << "Information present in machine state after execution: ";

        switch (measure) {
            case Measure::HAMMING_WEIGHT:
                std::cerr << count_entropy<hamming_weight>(original_memory, machine) << " Bits in non-zero state.";
                break;

            case Measure::WORD_DIFFERENCE: {
                const unsigned long long generated_entropy = count_entropy<word_difference>(original_memory, machine);
                std::cerr << generated_entropy * 32 << " Bits in " << generated_entropy << " 32-bit words.";
                break;
            }

            default:
                return;
        }

        std::cout << std::endl;
    }
}

