
#include <iostream>
#include "debugger.h"

namespace Machine {

    static continue_t debug_info(VM &vm, debugger_state &state, const std::vector<std::string> &args);

    static continue_t debug_step(VM &vm, debugger_state &state, const std::vector<std::string> &args);

    static continue_t debug_run(VM &vm, debugger_state &state, const std::vector<std::string> &args);

    static continue_t debug_breakpoint_create(VM &vm, debugger_state &state, const std::vector<std::string> &args);

    static continue_t debug_breakpoint_clear(VM &vm, debugger_state &state, const std::vector<std::string> &args);

    static continue_t debug_breakpoint_list(VM &vm, debugger_state &state, const std::vector<std::string> &args);

    static continue_t debug_inspect(VM &vm, debugger_state &state, const std::vector<std::string> &args);

    static continue_t debug_set(VM &vm, debugger_state &state, const std::vector<std::string> &args);

    static continue_t debug_quit(VM &vm, debugger_state &state, const std::vector<std::string> &args);

    std::map<std::string, debugger_command> available_commands = {
            {"info",       debug_info},
            {"step",       debug_step},
            {"s",          debug_step},
            {"run",        debug_run},
            {"r",          debug_run},
            {"continue",   debug_run},
            {"c",          debug_run},
            {"breakpoint", debug_breakpoint_create},
            {"break",      debug_breakpoint_create},
            {"b",          debug_breakpoint_create},
            {"clear",      debug_breakpoint_clear},
            {"list",       debug_breakpoint_list},
            {"inspect",    debug_inspect},
            {"i",          debug_inspect},
            {"set",        debug_set},
            {"quit",       debug_quit},
            {"q",          debug_quit},
    };


    static continue_t debug_info(VM &vm, debugger_state &, const std::vector<std::string> &) {
        print_machine_state(vm);
        return PROMPT_USER;
    }


    static continue_t debug_step(VM &, debugger_state &state, const std::vector<std::string> &args) {
        uint32_t steps = 1;
        if (args.size() >= 2) {
            try {
                steps = std::stoi(args[1]);
            } catch (std::logic_error &exception) {
                std::cout << "`" << args[1] << "' is not a valid number: " << exception.what() << std::endl;
            }
        }
        state.remaining_steps = steps;
        return CONTINUE_EXECUTION;
    }

    static continue_t debug_run(VM &, debugger_state &state, const std::vector<std::string> &) {
        state.continue_running = true;
        return CONTINUE_EXECUTION;
    }


    static continue_t debug_breakpoint_create(VM &, debugger_state &state, const std::vector<std::string> &args) {
        for (size_t index = 1; index < args.size(); index++) {
            try {
                const int32_t breakpoint_index = std::stoi(args[index]);
                if (state.breakpoints.insert(breakpoint_index).second) {
                    std::cout << "Created breakpoint at " << breakpoint_index << "." << std::endl;
                }
            } catch (std::exception &exception) {
                std::cout << "Failed to create breakpoint: " << exception.what() << std::endl;
            }
        }
        return PROMPT_USER;
    }

    static continue_t debug_breakpoint_clear(VM &, debugger_state &state, const std::vector<std::string> &args) {
        for (size_t index = 1; index < args.size(); index++) {
            try {
                const int32_t breakpoint_index = std::stoi(args[index]);
                if (state.breakpoints.erase(breakpoint_index)) {
                    std::cout << "Removed breakpoint at " << breakpoint_index << "." << std::endl;
                }
            } catch (std::exception &exception) {
                std::cout << "Failed to remove breakpoint: " << exception.what() << std::endl;
            }
        }
        return PROMPT_USER;
    }

    static continue_t debug_breakpoint_list(VM &, debugger_state &state, const std::vector<std::string> &) {
        if (state.breakpoints.empty()) {
            std::cout << "No breakpoints set." << std::endl;
        } else {
            std::cout << "There are " << state.breakpoints.size() << " active breakpoints: " << std::endl;
            for (const auto &item: state.breakpoints) {
                std::cout << " at line " << item << std::endl;
            }
        }
        return PROMPT_USER;
    }


    static int32_t &component_from_string(const std::string &string, VM &vm) {
        if (string == "sp") {
            return vm.sp;

        } else if (string == "fp") {
            return vm.fp;

        } else if (string == "br") {
            return vm.br;

        } else if (string == "pc") {
            return vm.pc;

        } else if (string == "dir") {
            return reinterpret_cast<int32_t &>(vm.dir);

        } else if (string.starts_with("S[") || string.starts_with("M[")) {
            std::vector<int32_t> &memory_component =
                    (string.starts_with('S')) ? vm.stack : vm.memory;

            const int32_t addr = std::stoi(string.substr(2, string.size() - 1));
            return memory_component.at(addr);
        }

        throw std::invalid_argument(std::string("Specifier `") + string + "' does not describe a machine component.");
    }

    static continue_t debug_inspect(VM &vm, debugger_state &, const std::vector<std::string> &args) {
        if (args.size() <= 1) {
            std::cout << "Please specify the machine component you want to inspect." << std::endl;
        } else {
            for (size_t index = 1; index < args.size(); index++) {
                try {
                    const int32_t value = component_from_string(args[index], vm);
                    std::cout << " " << args[index] << " = " << value << std::endl;
                } catch (std::exception &exception) {
                    std::cout << "Failed inspect value: " << exception.what() << std::endl;
                }
            }
        }
        return PROMPT_USER;
    }

    static continue_t debug_set(VM &vm, debugger_state &, const std::vector<std::string> &args) {
        for (size_t index = 1; index + 1 < args.size(); index += 2) {
            try {
                int32_t &component = component_from_string(args[index], vm);
                component = std::stoi(args[index + 1]);
                std::cout << " " << args[index] << " = " << component << std::endl;
            } catch (std::exception &exception) {
                std::cout << "Failed to set value: " << exception.what() << std::endl;
            }
        }
        return PROMPT_USER;
    }


    static continue_t debug_quit(VM &, debugger_state &state, const std::vector<std::string> &) {
        state.exit = true;
        return CONTINUE_EXECUTION;
    }
}
