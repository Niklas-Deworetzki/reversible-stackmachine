#pragma once

#include <set>
#include "machine/machine.h"

namespace Machine {
    void run_with_debugger(VM &vm);


    struct debugger_state {
        std::set<bool> breakpoints;
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

    typedef continue_t (*debugger_command)(VM &, debugger_state &, const std::vector<std::string> &);


    extern std::map<std::string, debugger_command> available_commands;

}

