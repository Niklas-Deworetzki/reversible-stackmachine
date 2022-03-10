#pragma once

#include <string>
#include <vector>
#include <functional>

extern "C" {
#include "csyntax.h"
};

using Section = std::vector<Line>;

struct Program {
    Section code;
    Section data;
    Section bss;

public:
    explicit Program(ParsedProgram parsed_program) noexcept;
};

[[nodiscard]] Program parse_file(const std::string &filename);


void iterate_section(Section &section, const std::function<void(Line &)> &action);

void iterate_section(const Section &section, const std::function<void(const Line &)> &action);

