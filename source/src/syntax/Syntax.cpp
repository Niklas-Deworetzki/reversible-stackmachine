
#include <string>
#include <stdexcept>
#include <filesystem>

#include "Syntax.h"
#include "Instructions.h"
#include "scanner.h"
#include "messages/error.h"

extern "C" int yyparse(ParsedProgram *, unsigned int *);

static void initialize_instruction(Instruction &instruction) noexcept {
    instruction.data = &InstructionData::get(instruction.offset, instruction.is_forward);
}

/**
 * Consumes a LineList, filling its element into a std::vector.
 * All LineList nodes are freed during this operation.
 */
static void toVector(std::vector<Line> &vector, LineList *list) noexcept {
    size_t size = 0;
    for (LineList *head = list; !head->isEmpty; head = head->tail) {
        size++;
    }

    vector.reserve(size);
    while (!list->isEmpty) {
        vector.push_back(list->head);

        LineList *ptr = list;
        list = list->tail;
        free(ptr);
    }
}

Program::Program(ParsedProgram parsed_program) noexcept {
    toVector(this->code, parsed_program.code);
    toVector(this->data, parsed_program.data);
    toVector(this->bss, parsed_program.bss);
}

[[nodiscard]] Program parse_file(const std::string &filename) {
    std::filesystem::path path = std::filesystem::absolute(filename);

    yyin = fopen(path.c_str(), "r");
    if (yyin == nullptr) {
        throw std::invalid_argument("File " + path.string() + " cannot be opened.");
    } else {
        ParsedProgram program;
        unsigned int detected_errors = 0;

        if (yyparse(&program, &detected_errors) != 0 || detected_errors > 0) {
            throw parse_error(detected_errors);
        }

        for (LineList *list = program.code; !list->isEmpty; list = list->tail) {
            if (list->head.variant == LINE_INSTRUCTION) {
                initialize_instruction(list->head.value.instruction);
            }
        }

        return Program(program);
    }
}


void iterate_section(Section &section, const std::function<void(Line &)> &action) {
    int32_t linenumber;

    try {
        for (auto &line: section) {
            linenumber = line.linenumber;
            action(line);
        }
    } catch (error_message &error_message) {
        error_message.setLineNumber(linenumber);
        throw;
    }
}

void iterate_section(const Section &section, const std::function<void(const Line &)> &action) {
    int32_t linenumber;

    try {
        for (const auto &line: section) {
            linenumber = line.linenumber;
            action(line);
        }
    } catch (error_message &error_message) {
        error_message.setLineNumber(linenumber);
        throw;
    }
}
