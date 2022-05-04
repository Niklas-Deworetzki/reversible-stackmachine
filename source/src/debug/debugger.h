#pragma once

#include <set>
#include "machine/machine.h"

namespace Machine {
    void run_with_debugger(VM &vm);

    void print_machine_state(VM &vm);


    struct debugger_state {
        std::set<int32_t> breakpoints;
        uint32_t remaining_steps;
        bool continue_running;
        std::string last_input;
        bool exit;

        debugger_state() : breakpoints(),
                           remaining_steps(0),
                           continue_running(false),
                           last_input(),
                           exit(false) {};
    };

    enum continue_t {
        PROMPT_USER,
        CONTINUE_EXECUTION
    };

    typedef continue_t (*debugger_function)(VM &, debugger_state &, const std::vector<std::string> &);

    struct debugger_command {
        const debugger_function implementation;
        const std::string name;
        const std::string description;
        const std::vector<std::string> aliases;
    };

    extern const std::vector<debugger_command> available_commands;

}

