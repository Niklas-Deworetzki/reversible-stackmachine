
#include <iostream>
#include <chrono>
#include <cmath>
#include <cstring>
#include <algorithm>
#include "syntax/Syntax.h"
#include "assembler/Assembler.h"
#include "machine/Machine.h"

#define VERSION_NUMBER "1.2"
static const char *version_string = "Stackmachine VM version " VERSION_NUMBER " (compiled " __DATE__ ")";

static const char *help_page =
        "Supported options are:\n"
        " -h, --help\n"
        "\tPrint this help page and exit.\n"
        " -v, --version\n"
        "\tPrint version information and exit.\n"
        " -i, --information\n"
        "    Print runtime information like execution time, time required to load the\n"
        "    program and executed instructions per second.\n"
        " -q, --quiet\n"
        "    Do not output anything if the stack is empty after the program finished.\n"
        " -s, --stacksize [SIZE]\n"
        "    Configures the amount of values the operand stack can hold.\n"
        " -m, --memsize [SIZE]\n"
        "    Configures the amount of values the program memory can hold.\n"
        " -S, --start [SYMBOL]\n"
        "    Starts the execution of a program at the given label. If this label is\n"
        "    not defined within the program, execution will begin at address 0.\n"
        "\n"
        "Both [SIZE] arguments may be any number, optionally suffixed by a size unit.\n"
        "Supported units are: k (1024^1), m (1024^2), g (1024^3).\n"
        "\n"
        "After successful execution, this program will print the elements on the\n"
        "argument stack and exit with an exit code of 0. If any error occurs during\n"
        "runtime, the exit code will be 1. If the command line arguments provided\n"
        "represent an invalid configuration, the exit code will be 2.\n"
        "\n"
        "(C) 2022, Niklas Deworetzki";


using namespace Assembler;
using std::cout, std::cerr, std::endl;


static void display_version() {
    cout << version_string << endl;
}

static void display_help(const char *progname) {
    cout << "A virtual machine for reversible stack machine programs.\n\n";
    cout << "  " << progname << " " << "[OPTIONS] FILE\n\n";
    cout << help_page << endl;
}

static void report_runtime_statistics(const Machine::VM &machine,
                                      const std::chrono::duration<double> load_time,
                                      const std::chrono::duration<double> run_time) noexcept {
    cerr.precision(2);
    cerr << std::fixed;
    cerr << "Loaded program in " << (load_time.count() * 1000) << "ms.\n";
    cerr << "Executed " << machine.counter << " instructions in " << (run_time.count() * 1000) << "ms" <<
         " (~ " << (long) floor((double) machine.counter / run_time.count()) << " instr/s)\n";
    cerr << endl;
}


static bool matches(const char *arg, std::initializer_list<const char *> potential_matches) {
    return std::ranges::any_of(potential_matches, [&arg](const char *element) {
        return strcmp(arg, element) == 0;
    });
}

static bool parse_size(const char *arg, size_t &result) {
    size_t accumulator = result = 0;

    for (; *arg; arg++) {
        char digit = *arg;

        if (isdigit(digit)) {
            accumulator = accumulator * 10 + (digit - '0');
        } else {
            switch (digit) {
                case 'g':
                case 'G':
                    accumulator *= 1024;
                case 'm':
                case 'M':
                    accumulator *= 1024;
                case 'k':
                case 'K':
                    accumulator *= 1024;
                    result = accumulator;
                    return true;

                default:
                    return false;
            }
        }
    }

    result = accumulator;
    return true;
}

#define REQUIRES_ARGS(n)                                                            \
    if (i + (n) >= argc) {                                                          \
        cerr << "Option requires at least " << (n) << "more arguments!" << endl;    \
        user_error = true;                                                          \
        continue;                                                                   \
    }                                                                               \


int main(int argc, char *argv[]) {
    const char *input_file = nullptr;
    std::optional<std::string> entry_point;
    bool should_display_help = false,
            should_display_version = false,
            should_display_info = false,
            should_be_quiet = false,
            path_separator = false,
            user_error = false;
    size_t memory_size = 102400,
            stack_size = 1024;

    for (int i = 1; i < argc; i++) {
        const char *current_arg = argv[i];

        if (!path_separator && matches(current_arg, {"--help", "-h"})) {
            should_display_help = true;
        } else if (!path_separator && matches(current_arg, {"--version", "-v"})) {
            should_display_version = true;
        } else if (!path_separator && matches(current_arg, {"--information", "-i"})) {
            should_display_info = true;
        } else if (!path_separator && matches(current_arg, {"--quiet", "-q"})) {
            should_be_quiet = true;

        } else if (!path_separator && matches(current_arg, {"--stacksize", "-s"})) {
            REQUIRES_ARGS(1)
            i += 1;
            if (!parse_size(argv[i], stack_size)) {
                cerr << "Invalid stack size: " << argv[i] << endl;
                user_error = true;
            }
        } else if (!path_separator && matches(current_arg, {"--memorysize", "--memsize", "-m"})) {
            REQUIRES_ARGS(1)
            i += 1;
            if (!parse_size(argv[i], memory_size)) {
                cerr << "Invalid memory size: " << argv[i] << endl;
                user_error = true;
            }
        } else if (!path_separator && matches(current_arg, {"--start", "-S"})) {
            REQUIRES_ARGS(1)
            i += 1;
            entry_point = argv[i];
        } else if (!path_separator && strcmp(current_arg, "--") == 0) {
            path_separator = true;
        } else if (!path_separator && current_arg[0] == '-') {
            user_error = true;
            cerr << "Unknown option: " << current_arg << endl;

        } else if (input_file == nullptr) {
            input_file = current_arg;
        } else {
            user_error = true;
            cerr << "Only a single input file is allowed. "
                    "Provided: '" << input_file << "' and '" << current_arg << "' !" << endl;
        }
    }

    if (should_display_help) {
        display_help(argv[0]);
    } else if (should_display_version) {
        display_version();
    }
    if (should_display_help || should_display_version)
        return 0;

    if (input_file == nullptr) {
        user_error = true;
        cerr << "No input file" << endl;
    }
    if (user_error)
        return 2;

    try {
        const auto start = std::chrono::high_resolution_clock::now();

        Program program = parse_file(input_file);
        const auto &[memory, code, entry_address] = assemble(program, entry_point);
        Machine::VM machine(code, memory, memory_size, stack_size, entry_address);

        const auto loaded = std::chrono::high_resolution_clock::now();

        machine.run();

        const auto finished = std::chrono::system_clock::now();
        if (should_display_info) {
            report_runtime_statistics(machine, loaded - start, finished - loaded);
        }

        if (machine.sp == 0) {
            if (!should_be_quiet) {
                cout << "Stack is empty." << endl;
            }
        } else {
            for (int i = machine.sp - 1; i >= 0; i--) {
                cout << machine.stack[i] << endl;
            }
        }

        if (machine.state == Machine::ILLEGAL_INSTRUCTION)
            return 1;
        else
            return 0;

    } catch (std::exception &exception) {
        cerr << "[ERROR] " << exception.what() << endl;
        return 1;
    }
}
