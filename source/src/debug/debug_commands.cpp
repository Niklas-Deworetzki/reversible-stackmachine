
#include <iostream>
#include "machine/machine.h"
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

    static continue_t debug_invert(VM &vm, debugger_state &state, const std::vector<std::string> &args);

    static continue_t debug_quit(VM &vm, debugger_state &state, const std::vector<std::string> &args);

    static continue_t debug_help(VM &vm, debugger_state &state, const std::vector<std::string> &args);

    const std::vector<debugger_command> available_commands = {
            {debug_info,              "info",       "Shows information about the machine state.",
                    {}},
            {debug_step,              "step",       "Execute the next instruction.",
                    {"s"}},
            {debug_run,               "run",        "Execute instructions until a breakpoint is hit or the program terminates",
                    {"r",     "continue", "c"}},
            {debug_breakpoint_create, "breakpoint", "Create a breakpoint at the given instruction.",
                    {"break", "b"}},
            {debug_breakpoint_clear,  "clear",      "Clears a breakpoint at the given instruction.",
                    {}},
            {debug_breakpoint_list,   "list",       "List all created breakpoints.",
                    {}},
            {debug_inspect,           "inspect",    "Inspect the value of a machine component.",
                    {"i"}},
            {debug_set,               "set",        "Set the value of a machine component.",
                    {}},
            {debug_invert,            "invert",     "Inverts the execution direction of the machine.",
                    {}},
            {debug_quit,              "quit",       "Exits the debugger, terminating the program.",
                    {"q"}},
            {debug_help,              "help",       "Display an overview of available commands.",
                    {}}
    };


    static void invert_vm_direction(VM &vm) {
        vm.dir = !vm.dir;
        vm.step_pc();
    }


    static continue_t debug_info(VM &vm, debugger_state &, const std::vector<std::string> &) {
        print_machine_state(vm);
        return PROMPT_USER;
    }


    static continue_t debug_step(VM &vm, debugger_state &state, const std::vector<std::string> &args) {
        int32_t steps = 1;
        if (args.size() >= 2) {
            try {
                steps = std::stoi(args[1]);
            } catch (std::logic_error &exception) {
                std::cout << "`" << args[1] << "' is not a valid number: " << exception.what() << std::endl;
            }
        }

        if (steps < 0) {
            invert_vm_direction(vm);
            state.remaining_steps = static_cast<uint32_t>(-steps);
        } else {
            state.remaining_steps = static_cast<uint32_t>(steps);
        }

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


    static const int32_t &component_from_string(const std::string &string, VM &vm, bool is_write_access = false) {
        if (string == "sp") {
            return vm.sp;

        } else if (string == "fp") {
            return vm.fp;

        } else if (string == "br") {
            return vm.br;

        } else if (string == "pc") {
            return vm.pc;

        } else if (string.starts_with("S[") || string.starts_with("M[") || string.starts_with("P[")) {
            const int32_t addr = std::stoi(string.substr(2, string.size() - 1));
            switch (string[0]) {
                case 'S':
                    return vm.memory[addr];

                case 'M':
                    return vm.stack[addr];

                case 'P':
                    if (is_write_access) {
                        throw std::invalid_argument("Writing to program memory is not allowed!");
                    }
                    return vm.program[addr];

            }
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
                auto &component = const_cast<int32_t &>(component_from_string(args[index], vm, true));
                component = std::stoi(args[index + 1]);
                std::cout << " " << args[index] << " = " << component << std::endl;
            } catch (std::exception &exception) {
                std::cout << "Failed to set value: " << exception.what() << std::endl;
            }
        }
        return PROMPT_USER;
    }


    static continue_t debug_invert(VM &vm, debugger_state &, const std::vector<std::string> &) {
        invert_vm_direction(vm);
        std::cout << "Direction is now " << ((vm.dir == Direction::Forward) ? "Forward" : "Backward") << "."
                  << std::endl;
        return PROMPT_USER;
    }


    static continue_t debug_quit(VM &, debugger_state &state, const std::vector<std::string> &) {
        state.exit = true;
        return CONTINUE_EXECUTION;
    }


    static continue_t debug_help(VM &, debugger_state &, const std::vector<std::string> &) {
        for (const auto &command: available_commands) {
            printf("%-10s - %s\n", command.name.c_str(), command.description.c_str());
        }
        return PROMPT_USER;
    }
}
