/**
 * This header file provides types and functions for the abstract stack assembler syntax.
 *
 * It is usable in C as well as C++.
 */

#pragma once

#include <inttypes.h>
#include <stdbool.h>

struct InstructionData;

typedef struct string_list {
    bool isEmpty;
    char *head;
    struct string_list *tail;
} StringList;


typedef enum {
    NO_OPERAND, PRIMITIVE_SYMBOL, PRIMITIVE_CONSTANT, PRIMITIVE_RELATIVE, COMPLEX_ADD, COMPLEX_SUB
} Variant;

typedef struct {
    Variant variant;
    union {
        const char *symbol;
        int32_t constant;
        int32_t relative;
    } value;
} PrimitiveOperand;

typedef struct {
    Variant variant;
    PrimitiveOperand lhs;
    PrimitiveOperand rhs;
} ComplexOperand;

typedef struct {
    Variant variant;
    union {
        PrimitiveOperand primitive;
        ComplexOperand complex;
    } instance;
} Operand;


typedef struct operand_list {
    bool isEmpty;
    Operand head;
    struct operand_list *tail;
} OperandList;


typedef struct {
    size_t offset;
    bool is_forward;
    Operand operand;
    const struct InstructionData *data;
} Instruction;

typedef struct {
    Operand size;
} ReservedWords;

typedef struct {
    OperandList *data;
} Words;

typedef struct {
    Operand memoryAddress;
    Operand value;
} SetValue;


typedef enum {
    LINE_INSTRUCTION = 1, LINE_RESERVED = 2, LINE_WORDS = 4, LINE_SET = 8
} LineVariant;

typedef struct {
    LineVariant variant;
    StringList *labels;
    int32_t linenumber, base_address;
    union {
        Instruction instruction;
        ReservedWords reserved;
        Words words;
        SetValue setValue;
    } value;
} Line;

typedef struct line_list {
    bool isEmpty;
    Line head;
    struct line_list *tail;
} LineList;

typedef struct {
    LineList *code;
    LineList *data;
    LineList *bss;
} ParsedProgram;


StringList *emptyStringList();

StringList *prependString(char *string, StringList *tail);

OperandList *emptyOperandList();

OperandList *prependOperand(Operand operand, OperandList *tail);

LineList *emptyLineList();

LineList *prependLine(Line line, LineList *tail);

LineList *merge(LineList *a, LineList *b);

Operand noOperand();

PrimitiveOperand mkSymbolic(const char *value);

PrimitiveOperand mkConstant(int32_t value);

PrimitiveOperand mkRelative(int32_t value);

Operand mkPrimitive(PrimitiveOperand primitive);

Operand mkAdd(PrimitiveOperand lhs, PrimitiveOperand rhs);

Operand mkSub(PrimitiveOperand lhs, PrimitiveOperand rhs);


Line mkInstruction(size_t offset, bool is_forward, Operand operand);

Line mkReservedWords(Operand size);

Line mkWords(OperandList *data);

Line mkSetValue(Operand memoryAddress, Operand value);

