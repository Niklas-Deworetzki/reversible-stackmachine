
#pragma once

#include <cstdint>
#include <stack>
#include <vector>
#include "assembler/Assembler.h"

namespace Machine {

    using Assembler::MemoryLayout;

    enum Direction : int32_t {
        Forward = 1, Backward = -1
    };

    [[nodiscard]] constexpr Direction operator!(Direction direction) noexcept;

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
