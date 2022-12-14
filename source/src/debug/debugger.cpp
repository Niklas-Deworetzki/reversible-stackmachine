
#include <iostream>
#include "debugger.h"

namespace Machine {

    void print_machine_state(VM &vm) {
        std::string counter_digits = std::to_string(vm.counter);
        if (counter_digits.size() < 3) {
            counter_digits = std::string(3 - counter_digits.size(), ' ') + counter_digits;
        }

        // dir = %s, pc = %d, br = %d, sp = %d, fp = %d
        std::cout << "[" << counter_digits << "]"
                  << " dir = " << (vm.dir == Direction::Forward ? "Forward" : "Backward")
                  << ", pc = " << vm.pc
                  << ", br = " << vm.br
                  << ", sp = " << vm.sp
                  << ", fp = " << vm.fp
                  << std::endl;

        std::cout << "Stack: ";

        int32_t lower_bound;
        if (vm.fp > 0) { // Print current stack frame.
            lower_bound = vm.fp;
        } else { // Print at most 10 elements of stack.
            lower_bound = std::max(0, vm.sp - 10);
        }

        for (int32_t addr = vm.sp - 1; addr >= lower_bound; addr--) {
            std::cout << vm.stack[addr] << " ";
        }

        if (lower_bound > 0) { // Indicate that not all elements have been printed.
            std::cout << "...";
        }

        std::cout << std::endl;
    }

    static bool requires_user_interaction(const VM &vm, const debugger_state &state);

    static void interact_with_user(VM &vm, debugger_state &state);

    static void step_debugger_state(debugger_state &state);


    void run_with_debugger(VM &vm) {
        debugger_state state;
        do {
            if (requires_user_interaction(vm, state)) {
                // We aren't running anymore.
                state.continue_running = false;

                interact_with_user(vm, state);
                if (state.exit) { // Check if user requested exit.
                    break;
                }
            }

            vm.step();
            step_debugger_state(state);
        } while (vm.running);
    }


    static bool requires_user_interaction(const VM &vm, const debugger_state &state) {
        if (state.continue_running) return state.breakpoints.contains(vm.pc);
        else return state.remaining_steps == 0;
    }

    static void step_debugger_state(debugger_state &state) {
        if (state.remaining_steps > 0) {
            state.remaining_steps--;
        }
    }


    static void split(const std::string &str, std::vector<std::string> &result_buffer) {
        std::istringstream stream(str);

        std::string token;
        while (std::getline(stream, token, ' ')) {
            if (!token.empty()) {
                result_buffer.push_back(token);
            }
        }
    }

    static std::optional<debugger_command> find_command(const std::string &command_name) {
        for (const auto &command: available_commands) {
            if (command.name == command_name) return command;
            for (const auto &alias: command.aliases) {
                if (alias == command_name) return command;
            }
        }
        return {};
    }

    static void interact_with_user(VM &vm, debugger_state &state) {
        std::string input;
        std::vector<std::string> command;

        print_machine_state(vm);

        continue_t may_continue = PROMPT_USER;
        do {
            std::cout << "> " << std::flush;
            std::getline(std::cin, input);

            if (std::cin.eof()) {
                std::cout << "Input closed. Exiting.";
                state.exit = true;
            } else {
                if (input.empty()) { // Try to re-perform last action.
                    if (state.last_input.empty()) {
                        continue; // No previous input cached. Prompt user again.
                    } else {
                        input = state.last_input;
                    }
                } else {
                    state.last_input = input;
                }

                command.clear(); // Reset command buffer.
                split(input, command);

                std::optional<debugger_command> executed_command = find_command(command[0]);
                if (executed_command.has_value()) {
                    may_continue = executed_command.value().implementation(vm, state, command);
                }
            }
        } while (may_continue == PROMPT_USER && !state.exit);
    }
}
