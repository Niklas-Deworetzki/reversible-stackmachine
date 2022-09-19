#pragma once

#include <string>
#include <utility>
#include <sstream>

class error_message : public std::exception {
    std::string _msg;
    int32_t _linenumber;

public:
    explicit error_message(std::string msg) : _msg(std::move(msg)), _linenumber(-1) {
    }

    ~error_message() override = default;

    [[nodiscard]] const std::string &getMessage() const noexcept {
        return _msg;
    }

    [[nodiscard]] const char *what() const noexcept override {
        return this->getMessage().c_str();
    }

    void setLineNumber(int32_t linenumber) {
        if (!hasLineNumber()) {
            this->_linenumber = linenumber;
            this->_msg = "Line " + std::to_string(linenumber) + ": " + _msg;
        }
    }

    [[nodiscard]] int32_t getLineNumber() const noexcept {
        return this->_linenumber;
    }

    [[nodiscard]] bool hasLineNumber() const noexcept {
        return this->_linenumber >= 0;
    }
};


class parse_error : public error_message {
public:
    explicit parse_error(unsigned int count) : error_message(
            "Detected " + std::to_string(count) + " syntax error(s).") {}

    ~parse_error() override = default;
};

class symbol_redefinition_error : public error_message {
public:
    explicit symbol_redefinition_error(const char *symbol) : error_message(
            "Redefinition of '" + std::string(symbol) + "' is not allowed!") {}

    ~symbol_redefinition_error() override = default;
};

class set_address_clash : public error_message {
public:
    explicit set_address_clash(const int32_t requested_address) : error_message(
            "Directive .set clashes on address " + std::to_string(requested_address)) {}

    ~set_address_clash() override = default;
};

class invalid_operand : public error_message {
public:
    explicit invalid_operand(const char *message) : error_message(
            std::string("Operand cannot be evaluated: ") + message) {}

    ~invalid_operand() override = default;
};

class illegal_section_content : public error_message {
public:
    explicit illegal_section_content() : error_message(
            "Section contains illegal content.") {}

    ~illegal_section_content() override = default;
};

class start_stop_presence : public error_message {
public:
    explicit start_stop_presence(const char *mnemonic) : error_message(
            std::string("Programs must define exactly 1 ") + mnemonic + " instruction.") {}
};

static std::string hex_format(int32_t instruction) {
    std::stringstream stream;
    stream << std::hex << instruction;
    return stream.str();
}

class out_of_memory : public error_message {
public:
    explicit out_of_memory(int min_required, int max_required) : error_message(
            "Insufficient memory allocated for program execution. Program expects at least " +
                    std::to_string((size_t) max_required - min_required) + " bytes of memory.") {}
};

class illegal_instruction : public error_message {
public:
    explicit illegal_instruction(int32_t instruction, int32_t opcode) : error_message(
            "Cannot execute illegal instruction" + hex_format(instruction) +
            " with opcode " + hex_format(opcode) + ".") {}
};