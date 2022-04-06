#pragma once

#include "assembler/Assembler.h"
#include "machine/Machine.h"

namespace Entropy {

    enum class Measure {
        NONE, HAMMING_WEIGHT, WORD_DIFFERENCE
    };

    void report_entropy(Measure measure,
                        const Assembler::MemoryLayout &original_memory,
                        const Machine::VM &machine);

}