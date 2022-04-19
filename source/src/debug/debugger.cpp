
#include <set>
#include "debugger.h"

namespace Machine {

    struct debugger_state {
        std::set<bool> breakpoints;
        uint32_t remaining_steps;
        bool continue_running;
    };

    typedef bool (*debugger_hook)(VM &, debugger_state &);


    static bool requires_user_interaction(const VM &vm, const debugger_state &state);

    static void interact_with_user(VM &vm, debugger_state &state);

    static void step_debugger_state(debugger_state &state);

    void run_with_debugger(VM &vm) {
        debugger_state state{};
        do {
            if (requires_user_interaction(vm, state)) {
                interact_with_user(vm, state);
            }
            vm.step();
            step_debugger_state(state);
        } while (vm.running);
    }

}
