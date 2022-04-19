#pragma once

/**
 * This header provides functions to compute the amount of entropy present
 * in a machine state. 
 * Entropy is computed by counting the bits or words, having a value that
 * is not determined by machine state as described by the loaded program.
 */

#include "assembler/assembler.h"
#include "machine/machine.h"

namespace Entropy {

    enum class Measure {
        /**
         * Don't compute any entropy at all.
         */
        NONE, 
        /**
         * Compute the hamming weight of entropy present in machine state.
         * This will only count the Bits in words that are different from
         * their expected value.
         */
        HAMMING_WEIGHT, 
        /**
         * Compute the entropy present in the machine state by counting
         * all bits of a word, if the word's value is different from
         * its expected value.
         */
        WORD_DIFFERENCE
    };

    void report_entropy(Measure measure,
                        const Assembler::MemoryLayout &original_memory,
                        const Machine::VM &machine);

}