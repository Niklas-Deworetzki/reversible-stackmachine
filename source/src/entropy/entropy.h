#pragma once

#include "assembler/assembler.h"
#include "machine/machine.h"

namespace Entropy {

    enum class Measure {
        NONE, HAMMING_WEIGHT, WORD_DIFFERENCE
    };

    void report_entropy(Measure measure,
                        const Assembler::MemoryLayout &original_memory,
                        const Machine::VM &machine);

}