
#include <bit>
#include <cinttypes>
#include <iostream>
#include "assembler/assembler.h"
#include "entropy.h"
#include "machine/machine.h"

namespace Entropy {

    /**
     * Core function used to compute entropy. Parameterized with a template-parameter
     * representing the function used to determine the entropy difference between
     * two words.
     *
     * This function will compare the memory of the machine with its original 
     * memory layout as it was computed from the input program before execution.
     * The words at every address in the memory layout are passed to the template
     * parameter function. The same is done for every word on the stack and the
     * stack pointer, if the stack pointer is not zero.
     */
    template<unsigned int (entropy_generated_with_words)(int32_t, int32_t)>
    static unsigned long long count_entropy(const Assembler::MemoryLayout &original_memory,
                                            const Machine::VM &machine) {
        unsigned long long result = 0;
        // Compare values in memory.
        for (int32_t address = 0; (unsigned) address < machine.memory.size(); address++) {
            if (original_memory.contains(address)) {
                result += entropy_generated_with_words(machine.memory[address], original_memory.at(address));
            } else {
                result += entropy_generated_with_words(machine.memory[address], 0);
            }
        }
        // Compare values on stack.
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
