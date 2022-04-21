#pragma once

/**
 * Interface for the virtual machine implementation.
 */

#include <cstdint>
#include <stack>
#include <vector>
#include "assembler/assembler.h"

namespace Machine {

    using Assembler::MemoryLayout;

    enum Direction : int32_t /* Direction is stored as a machine word. */ {
        Forward = 1, 
        Backward = -1
    };

    /**
     * Negation operator for Direction inverts the direction.
     */
    [[nodiscard]] inline constexpr Direction operator!(const Direction direction) noexcept {
        return direction == Forward ? Backward : Forward;
    }

    struct VM {
        Direction dir;
        int32_t pc;
        int32_t br;

        int32_t sp;
        int32_t fp;

        std::vector<int32_t> memory;
        std::vector<int32_t> stack;

        bool running;
        size_t counter;

        const std::vector<int32_t> &program;

        explicit VM(const std::vector<int32_t> &program,
                    const MemoryLayout &memory_layout,
                    size_t memory_size,
                    size_t stack_size,
                    int32_t pc);

        void step();

        void run();

    private:
        void step_pc();

        void step_instr();
    };

}
