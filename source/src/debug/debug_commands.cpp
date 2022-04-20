
#include "debugger.h"

namespace Machine {

    static continue_t debug_step(VM &vm, debugger_state &state, const std::vector<std::string> &args);

    static continue_t debug_run(VM &vm, debugger_state &state, const std::vector<std::string> &args);

    static continue_t debug_breakpoint_create(VM &vm, debugger_state &state, const std::vector<std::string> &args);

    static continue_t debug_breakpoint_clear(VM &vm, debugger_state &state, const std::vector<std::string> &args);

    static continue_t debug_inspect(VM &vm, debugger_state &state, const std::vector<std::string> &args);

    static continue_t debug_set(VM &vm, debugger_state &state, const std::vector<std::string> &args);

    static continue_t debug_quit(VM &vm, debugger_state &state, const std::vector<std::string> &args);

    std::map<std::string, debugger_command> available_commands = {
            {"step",       debug_step},
            {"s",          debug_step},
            {"run",        debug_run},
            {"r",          debug_run},
            {"continue",   debug_run},
            {"breakpoint", debug_breakpoint_create},
            {"break",      debug_breakpoint_create},
            {"b",          debug_breakpoint_create},
            {"clear",      debug_breakpoint_clear},
            {"c",          debug_breakpoint_clear},
            {"inspect",    debug_inspect},
            {"i",          debug_inspect},
            {"set",        debug_set},
            {"quit",       debug_quit},
            {"q",          debug_quit},
    };
}
