
#include <stdbool.h>
#include <stddef.h>
#include <malloc.h>
#include <stdlib.h>
#include "csyntax.h"

static StringList nil_StringList = {
        .isEmpty = true
};

static OperandList nil_OperandList = {
        .isEmpty = true
};

static LineList nil_LineList = {
        .isEmpty = true
};

StringList *emptyStringList() {
    return &nil_StringList;
}

OperandList *emptyOperandList() {
    return &nil_OperandList;
}

LineList *emptyLineList() {
    return &nil_LineList;
}


static void *allocate(size_t size) {
    void *p = malloc(size);
    if (p == NULL) {
        printf("out of memory");
        exit(1);
    }
    return p;
}


StringList *prependString(char *head, StringList *tail) {
    StringList *cons = allocate(sizeof(StringList));
    cons->isEmpty = false;
    cons->head = head;
    cons->tail = tail;
    return cons;
}

OperandList *prependOperand(Operand operand, OperandList *tail) {
    OperandList *cons = allocate(sizeof(OperandList));
    cons->isEmpty = false;
    cons->head = operand;
    cons->tail = tail;
    return cons;
}

LineList *prependLine(Line line, LineList *tail) {
    LineList *cons = allocate(sizeof(LineList));
    cons->isEmpty = false;
    cons->head = line;
    cons->tail = tail;
    return cons;
}

LineList *merge(LineList *a, LineList *b) {
    if (a->isEmpty) return b;
    else {
        LineList *end = a;
        while (!end->tail->isEmpty) {
            end = end->tail;
        }
        end->tail = b;
        return a;
    }
}

Operand noOperand() {
    Operand operand = {.variant = NO_OPERAND};
    return operand;
}

PrimitiveOperand mkSymbolic(const char *value) {
    PrimitiveOperand prim = {
            .variant = PRIMITIVE_SYMBOL,
            .value.symbol = value
    };
    return prim;
}

PrimitiveOperand mkConstant(int32_t value) {
    PrimitiveOperand prim = {
            .variant = PRIMITIVE_CONSTANT,
            .value.constant = value
    };
    return prim;
}

PrimitiveOperand mkRelative(int32_t value) {
    PrimitiveOperand prim = {
            .variant = PRIMITIVE_RELATIVE,
            .value.relative = value
    };
    return prim;
}

Operand mkPrimitive(PrimitiveOperand primitive) {
    Operand operand = {.variant = primitive.variant};
    operand.instance.primitive = primitive;
    return operand;
}

Operand mkAdd(PrimitiveOperand lhs, PrimitiveOperand rhs) {
    Operand operand = {.variant = COMPLEX_ADD};
    ComplexOperand value = {
            .variant = COMPLEX_ADD,
            .lhs = lhs,
            .rhs = rhs,
    };
    operand.instance.complex = value;
    return operand;

}

Operand mkSub(PrimitiveOperand lhs, PrimitiveOperand rhs) {
    Operand operand = {.variant = COMPLEX_SUB};
    ComplexOperand value = {
            .variant = COMPLEX_SUB,
            .lhs = lhs,
            .rhs = rhs,
    };
    operand.instance.complex = value;
    return operand;
}


Line mkInstruction(size_t offset, bool is_forward, Operand operand) {
    Instruction instruction = {
            .offset = offset,
            .is_forward = is_forward,
            .operand = operand,
    };
    Line line = {
            .variant = LINE_INSTRUCTION,
    };
    line.value.instruction = instruction;
    return line;
}

Line mkReservedWords(Operand size) {
    ReservedWords words = {
            .size = size,
    };
    Line line = {
            .variant = LINE_RESERVED,
    };
    line.value.reserved = words;
    return line;
}

Line mkWords(OperandList *data) {
    Words words = {
            .data = data,
    };
    Line line = {
            .variant = LINE_WORDS,
    };
    line.value.words = words;
    return line;
}

Line mkSetValue(Operand memoryAddress, Operand value) {
    SetValue setValue = {
            .memoryAddress = memoryAddress,
            .value = value,
    };
    Line line = {
            .variant = LINE_SET,
    };
    line.value.setValue = setValue;
    return line;
}

