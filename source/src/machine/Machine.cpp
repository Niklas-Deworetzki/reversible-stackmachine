
#include <bit>
#include <stdexcept>
#include "Machine.h"
#include "syntax/Instructions.h"

#define CMPUSH(op) {                                                     \
    PUSHES_VALUES(1);                                                    \
    REQUIRES_PARAMS(2);                                                  \
    stack[sp] = (stack[sp - 1]) op (stack[sp - 2]) ? True : False;       \
    sp += 1;                                                             \
}
#define CMPOP(op) {                                                      \
    REQUIRES_PARAMS(3);                                                  \
    sp -= 1;                                                             \
    clear(stack[sp], (stack[sp - 1]) op (stack[sp - 2]) ? True : False); \
}
#define ARPUSH(op) {                                                     \
    PUSHES_VALUES(1);                                                    \
    REQUIRES_PARAMS(2);                                                  \
    stack[sp] = (stack[sp - 1]) op (stack[sp - 2]);                      \
    sp += 1;                                                             \
}
#define ARPOP(op) {                                                      \
    REQUIRES_PARAMS(3);                                                  \
    sp -= 1;                                                             \
    clear(stack[sp], (stack[sp - 1]) op (stack[sp - 2]));                \
}

#ifdef UNSAFE_OPERATIONS

#define clear(value, expected) value ^= expected
#define REQUIRES_PARAMS(n)
#define PUSHES_VALUES(n)
#define ASSERT_POSITIVE(n)

#else

static void clear(int32_t &value, int32_t expected) {
    value ^= expected;
    if (value) {
        std::string message = "Value is supposed to be " + std::to_string(expected) +
                              " but the actual value is " + std::to_string(expected ^ value) + ".";
        throw std::domain_error(message);
    }
}

#define REQUIRES_PARAMS(n) if (sp < (n)) throw std::underflow_error("Stack underflow. Not enough elements on the stack.")
#define PUSHES_VALUES(n) if (stack.capacity() - sp <= static_cast<size_t>(n)) throw std::overflow_error("Stack overflow. Capacity exceeded.")
#define ASSERT_POSITIVE(n) if ((n) < 0) throw std::invalid_argument("Negative operands are not supported for stack allocation instructions.")

#endif

namespace Machine {

    static constexpr bool strequal(const char *a, const char *b) {
        if (*a == '\0' && *b == '\0') return true;
        if (*a == '\0' || *b == '\0') return false;
        return *a == *b && strequal(a + 1, b + 1);
    }

    static constexpr int32_t opcodeFor(const char *mnemonic) {
        for (const auto &instruction: KNOWN_INSTRUCTIONS) {
            if (strequal(instruction.fw_mnemonic, mnemonic)) {
                return instruction.binary;
            } else if (strequal(instruction.bw_mnemonic, mnemonic)) {
                return INVERSE(instruction.binary);
            }
        }
        throw std::domain_error("Unknown instruction mnemonic!");
    }


    using std::swap;

    [[nodiscard]] constexpr Direction operator!(const Direction direction) noexcept {
        return direction == Forward ? Backward : Forward;
    }

    static constexpr int32_t True = Backward;
    static constexpr int32_t False = Forward;

    void VM::step_pc() {
        if (br == 0) {
            pc += dir;
        } else {
            pc += dir * br;
        }
    }

    void VM::step_instr() {
        const int32_t instruction = program.at(pc);
        const int32_t operand = sign_extend(instruction & OPERAND_WIDTH_MASK);
        const int32_t opcode = (instruction >> OPERAND_WIDTH) & OPCODE_WIDTH_MASK;

        switch (dir == Forward ? opcode : INVERSE(opcode)) {
            case opcodeFor("halt"):
            case INVERSE(opcodeFor("halt")):
                state = STOPPED;
                break;

            case opcodeFor("nop"):
            case INVERSE(opcodeFor("nop")):
                break;

            case opcodeFor("pushc"):
                PUSHES_VALUES(1);
                stack[sp] = operand;
                sp += 1;
                break;

            case opcodeFor("popc"):
                REQUIRES_PARAMS(1);
                sp -= 1;
                clear(stack[sp], operand);
                break;

            case opcodeFor("dup"):
                PUSHES_VALUES(1);
                REQUIRES_PARAMS(1);
                stack[sp] = stack[sp - 1];
                sp += 1;
                break;

            case opcodeFor("undup"):
                REQUIRES_PARAMS(2);
                sp -= 1;
                clear(stack[sp], stack[sp - 1]);
                break;

            case opcodeFor("swap"):
            case INVERSE(opcodeFor("swap")):
                REQUIRES_PARAMS(2);
                swap(stack[sp - 1], stack[sp - 2]);
                break;

            case opcodeFor("bury"): {
                REQUIRES_PARAMS(3);
                int32_t sp1(stack[sp - 1]), sp2(stack[sp - 2]), sp3(stack[sp - 3]);
                stack[sp - 3] = sp1;
                stack[sp - 2] = sp3;
                stack[sp - 1] = sp2;
                break;
            }
            case opcodeFor("dig"): {
                REQUIRES_PARAMS(3);
                int32_t sp1(stack[sp - 1]), sp2(stack[sp - 2]), sp3(stack[sp - 3]);
                stack[sp - 1] = sp3;
                stack[sp - 2] = sp1;
                stack[sp - 3] = sp2;
                break;
            }

            case opcodeFor("allocpar"):
                ASSERT_POSITIVE(operand);
                PUSHES_VALUES(operand);
                sp += operand;
                break;
            case opcodeFor("releasepar"):
                ASSERT_POSITIVE(operand);
                REQUIRES_PARAMS(operand);
                for (int i = 1; i <= operand; ++i) {
                    clear(stack[sp - i], 0);
                }
                sp -= operand;
                break;

            case opcodeFor("asf"):
                ASSERT_POSITIVE(operand);
                PUSHES_VALUES(operand + 1);
                stack[sp] = fp;
                fp = sp;
                sp += operand + 1;
                break;
            case opcodeFor("rsf"):
                ASSERT_POSITIVE(operand);
                REQUIRES_PARAMS(operand + 1);
                for (int i = 1; i <= operand; ++i) {
                    clear(stack[sp - i], 0);
                }
                sp -= operand + 1;

                clear(fp, sp);
                swap(fp, stack[sp]);
                break;

            case opcodeFor("pushl"):
                PUSHES_VALUES(1);
                swap(stack[sp], stack.at(fp + operand));
                sp += 1;
                break;
            case opcodeFor("popl"):
                REQUIRES_PARAMS(1);
                sp -= 1;
                swap(stack[sp], stack.at(fp + operand));
                clear(stack[sp], 0);
                break;

            case opcodeFor("call"):
            case INVERSE(opcodeFor("call")):
                REQUIRES_PARAMS(1);
                swap(br, stack[sp - 1]);
                break;

            case opcodeFor("uncall"):
            case INVERSE(opcodeFor("uncall")):
                REQUIRES_PARAMS(1);
                br = -br;
                stack[sp - 1] = -stack[sp - 1];
                swap(br, stack[sp - 1]);
                dir = !dir;
                break;

            case opcodeFor("branch"):
            case INVERSE(opcodeFor("branch")):
                br += dir * operand;
                break;

            case opcodeFor("brt"):
            case INVERSE(opcodeFor("brt")):
                REQUIRES_PARAMS(1);
                if (stack[sp - 1] == True) {
                    br += dir * operand;
                }
                break;

            case opcodeFor("brf"):
            case INVERSE(opcodeFor("brf")):
                REQUIRES_PARAMS(1);
                if (stack[sp - 1] == False) {
                    br += dir * operand;
                }
                break;

            case opcodeFor("pushtrue"):
                PUSHES_VALUES(1);
                stack[sp] = True;
                sp += 1;
                break;
            case opcodeFor("poptrue"):
                REQUIRES_PARAMS(1);
                sp -= 1;
                clear(stack[sp], True);
                break;

            case opcodeFor("pushfalse"):
                PUSHES_VALUES(1);
                stack[sp] = False;
                sp += 1;
                break;
            case opcodeFor("popfalse"):
                REQUIRES_PARAMS(1);
                sp -= 1;
                clear(stack[sp], False);
                break;

            case opcodeFor("cmpusheq"): CMPUSH(==)
                break;
            case opcodeFor("cmpopeq"): CMPOP(==)
                break;
            case opcodeFor("cmpushne"): CMPUSH(!=)
                break;
            case opcodeFor("cmpopne"): CMPOP(!=)
                break;
            case opcodeFor("cmpushlt"): CMPUSH(<)
                break;
            case opcodeFor("cmpoplt"): CMPOP(<)
                break;
            case opcodeFor("cmpushle"): CMPUSH(<=)
                break;
            case opcodeFor("cmpople"): CMPOP(<=)
                break;

            case opcodeFor("inc"):
                REQUIRES_PARAMS(1);
                stack[sp - 1] += operand;
                break;
            case opcodeFor("dec"):
                REQUIRES_PARAMS(1);
                stack[sp - 1] -= operand;
                break;

            case opcodeFor("neg"):
            case INVERSE(opcodeFor("neg")):
                REQUIRES_PARAMS(1);
                stack[sp - 1] = -stack[sp - 1];
                break;

            case opcodeFor("add"):
                REQUIRES_PARAMS(2);
                stack[sp - 1] += stack[sp - 2];
                break;
            case opcodeFor("sub"):
                REQUIRES_PARAMS(2);
                stack[sp - 1] -= stack[sp - 2];
                break;
            case opcodeFor("xor"):
            case INVERSE(opcodeFor("xor")):
                REQUIRES_PARAMS(2);
                stack[sp - 1] ^= stack[sp - 2];
                break;
            case opcodeFor("shl"): {
                REQUIRES_PARAMS(2);
                uint32_t value = *reinterpret_cast<uint32_t *>(&stack[sp - 1]);
                value = std::rotl(value, stack[sp - 2]);
                stack[sp - 1] = *reinterpret_cast<int32_t *>(&value);
                break;
            }
            case opcodeFor("shr"): {
                REQUIRES_PARAMS(2);
                uint32_t value = *reinterpret_cast<uint32_t *>(&stack[sp - 1]);
                value = std::rotr(value, stack[sp - 2]);
                stack[sp - 1] = *reinterpret_cast<int32_t *>(&value);
                break;
            }

            case opcodeFor("arpushadd"): ARPUSH(+)
                break;
            case opcodeFor("arpopadd"): ARPOP(+)
                break;
            case opcodeFor("arpushsub"): ARPUSH(-)
                break;
            case opcodeFor("arpopsub"): ARPOP(-)
                break;
            case opcodeFor("arpushmul"): ARPUSH(*)
                break;
            case opcodeFor("arpopmul"): ARPOP(*)
                break;
            case opcodeFor("arpushdiv"): ARPUSH(/)
                break;
            case opcodeFor("arpopdiv"): ARPOP(/)
                break;
            case opcodeFor("arpushmod"): ARPUSH(%)
                break;
            case opcodeFor("arpopmod"): ARPOP(%)
                break;
            case opcodeFor("arpushand"): ARPUSH(&)
                break;
            case opcodeFor("arpopand"): ARPOP(&)
                break;
            case opcodeFor("arpushor"): ARPUSH(|)
                break;
            case opcodeFor("arpopor"): ARPOP(|)
                break;

            case opcodeFor("pushm"):
                PUSHES_VALUES(1);
                swap(stack[sp], memory.at(operand));
                sp += 1;
                break;
            case opcodeFor("popm"):
                REQUIRES_PARAMS(1);
                sp -= 1;
                swap(stack[sp], memory.at(operand));
                clear(stack[sp], 0);
                break;

            case opcodeFor("load"):
                PUSHES_VALUES(1);
                REQUIRES_PARAMS(1);
                swap(stack[sp], memory.at(stack[sp - 1] + operand));
                sp += 1;
                break;
            case opcodeFor("store"):
                REQUIRES_PARAMS(2);
                sp -= 1;
                swap(stack[sp], memory.at(stack[sp - 1] + operand));
                clear(stack[sp], 0);
                break;

            case opcodeFor("memswap"):
            case INVERSE(opcodeFor("memswap")):
                REQUIRES_PARAMS(2);
                swap(memory.at(stack[sp - 1]), memory.at(stack[sp - 2]));
                break;

            case opcodeFor("xorhc"):
            case INVERSE(opcodeFor("xorhc")):
                REQUIRES_PARAMS(1);
                // Don't use operand here, since it is sign-extended and we want the raw bits.
                stack[sp - 1] ^= (instruction & OPCODE_WIDTH_MASK) << (OPERAND_WIDTH - 1);
                break;

            default:
                state = ILLEGAL_INSTRUCTION;
        }
    }


    void VM::step() {
        this->counter++;
        this->step_instr();
        this->step_pc();
    }

    void VM::run() {
        while (this->state == RUNNING)
            this->step();
    }

    VM::VM(const std::vector<int32_t> &program,
           const MemoryLayout &memory_layout,
           const size_t memory_size,
           const size_t stack_size,
           const int32_t pc) :
            dir(Forward), pc(pc), br(0), sp(0), fp(0),
            memory(memory_size), stack(stack_size),
            state(RUNNING), counter(0), program(program) {
        for (const auto &[address, value]: memory_layout) {
            memory.at(address) = value;
        }
    }
}
