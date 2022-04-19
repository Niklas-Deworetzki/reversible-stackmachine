
#include <bit>
#include <stdexcept>
#include "machine.h"
#include "syntax/instructions.h"

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
        size_t index = 0;
        do {
            if (a[index] != b[index]) return false;
            if (a[index] == '\0') return true;
            index++;
        } while (true);
    }

    static constexpr int32_t opcode_for(const char *mnemonic) {
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
            case opcode_for("start"):
                if (this->running) throw std::logic_error("Executed 'start' instruction on machine already running. Please ensure that only one 'start' instruction is executed per program.");
                else this->running = true;
                break;
            case opcode_for("stop"):
                if (!this->running) throw std::logic_error("Executed 'stop' instruction on machine that is not running. Please ensure that only one 'stop' instruction is executed per program.");
                else this->running = false;
                break;

            case opcode_for("nop"):
            case INVERSE(opcode_for("nop")):
                break;

            case opcode_for("pushc"):
                PUSHES_VALUES(1);
                stack[sp] = operand;
                sp += 1;
                break;

            case opcode_for("popc"):
                REQUIRES_PARAMS(1);
                sp -= 1;
                clear(stack[sp], operand);
                break;

            case opcode_for("dup"):
                PUSHES_VALUES(1);
                REQUIRES_PARAMS(1);
                stack[sp] = stack[sp - 1];
                sp += 1;
                break;

            case opcode_for("undup"):
                REQUIRES_PARAMS(2);
                sp -= 1;
                clear(stack[sp], stack[sp - 1]);
                break;

            case opcode_for("swap"):
            case INVERSE(opcode_for("swap")):
                REQUIRES_PARAMS(2);
                swap(stack[sp - 1], stack[sp - 2]);
                break;

            case opcode_for("bury"): {
                REQUIRES_PARAMS(3);
                int32_t sp1(stack[sp - 1]), sp2(stack[sp - 2]), sp3(stack[sp - 3]);
                stack[sp - 3] = sp1;
                stack[sp - 2] = sp3;
                stack[sp - 1] = sp2;
                break;
            }
            case opcode_for("dig"): {
                REQUIRES_PARAMS(3);
                int32_t sp1(stack[sp - 1]), sp2(stack[sp - 2]), sp3(stack[sp - 3]);
                stack[sp - 1] = sp3;
                stack[sp - 2] = sp1;
                stack[sp - 3] = sp2;
                break;
            }

            case opcode_for("allocpar"):
                ASSERT_POSITIVE(operand);
                PUSHES_VALUES(operand);
                sp += operand;
                break;
            case opcode_for("releasepar"):
                ASSERT_POSITIVE(operand);
                REQUIRES_PARAMS(operand);
                for (int i = 1; i <= operand; ++i) {
                    clear(stack[sp - i], 0);
                }
                sp -= operand;
                break;

            case opcode_for("asf"):
                ASSERT_POSITIVE(operand);
                PUSHES_VALUES(operand + 1);
                stack[sp] = fp;
                fp = sp;
                sp += operand + 1;
                break;
            case opcode_for("rsf"):
                ASSERT_POSITIVE(operand);
                REQUIRES_PARAMS(operand + 1);
                for (int i = 1; i <= operand; ++i) {
                    clear(stack[sp - i], 0);
                }
                sp -= operand + 1;

                clear(fp, sp);
                swap(fp, stack[sp]);
                break;

            case opcode_for("pushl"):
                PUSHES_VALUES(1);
                swap(stack[sp], stack.at(fp + operand));
                sp += 1;
                break;
            case opcode_for("popl"):
                REQUIRES_PARAMS(1);
                sp -= 1;
                swap(stack[sp], stack.at(fp + operand));
                clear(stack[sp], 0);
                break;

            case opcode_for("call"):
            case INVERSE(opcode_for("call")):
                REQUIRES_PARAMS(1);
                swap(br, stack[sp - 1]);
                break;

            case opcode_for("uncall"):
            case INVERSE(opcode_for("uncall")):
                REQUIRES_PARAMS(1);
                br = -br;
                stack[sp - 1] = -stack[sp - 1];
                swap(br, stack[sp - 1]);
                dir = !dir;
                break;

            case opcode_for("branch"):
            case INVERSE(opcode_for("branch")):
                br += dir * operand;
                break;

            case opcode_for("brt"):
            case INVERSE(opcode_for("brt")):
                REQUIRES_PARAMS(1);
                if (stack[sp - 1] == True) {
                    br += dir * operand;
                }
                break;

            case opcode_for("brf"):
            case INVERSE(opcode_for("brf")):
                REQUIRES_PARAMS(1);
                if (stack[sp - 1] == False) {
                    br += dir * operand;
                }
                break;

            case opcode_for("pushtrue"):
                PUSHES_VALUES(1);
                stack[sp] = True;
                sp += 1;
                break;
            case opcode_for("poptrue"):
                REQUIRES_PARAMS(1);
                sp -= 1;
                clear(stack[sp], True);
                break;

            case opcode_for("pushfalse"):
                PUSHES_VALUES(1);
                stack[sp] = False;
                sp += 1;
                break;
            case opcode_for("popfalse"):
                REQUIRES_PARAMS(1);
                sp -= 1;
                clear(stack[sp], False);
                break;

            case opcode_for("cmpusheq"): CMPUSH(==)
                break;
            case opcode_for("cmpopeq"): CMPOP(==)
                break;
            case opcode_for("cmpushne"): CMPUSH(!=)
                break;
            case opcode_for("cmpopne"): CMPOP(!=)
                break;
            case opcode_for("cmpushlt"): CMPUSH(<)
                break;
            case opcode_for("cmpoplt"): CMPOP(<)
                break;
            case opcode_for("cmpushle"): CMPUSH(<=)
                break;
            case opcode_for("cmpople"): CMPOP(<=)
                break;

            case opcode_for("inc"):
                REQUIRES_PARAMS(1);
                stack[sp - 1] += operand;
                break;
            case opcode_for("dec"):
                REQUIRES_PARAMS(1);
                stack[sp - 1] -= operand;
                break;

            case opcode_for("neg"):
            case INVERSE(opcode_for("neg")):
                REQUIRES_PARAMS(1);
                stack[sp - 1] = -stack[sp - 1];
                break;

            case opcode_for("add"):
                REQUIRES_PARAMS(2);
                stack[sp - 1] += stack[sp - 2];
                break;
            case opcode_for("sub"):
                REQUIRES_PARAMS(2);
                stack[sp - 1] -= stack[sp - 2];
                break;
            case opcode_for("xor"):
            case INVERSE(opcode_for("xor")):
                REQUIRES_PARAMS(2);
                stack[sp - 1] ^= stack[sp - 2];
                break;
            case opcode_for("shl"): {
                REQUIRES_PARAMS(2);
                uint32_t value = *reinterpret_cast<uint32_t *>(&stack[sp - 1]);
                value = std::rotl(value, stack[sp - 2]);
                stack[sp - 1] = *reinterpret_cast<int32_t *>(&value);
                break;
            }
            case opcode_for("shr"): {
                REQUIRES_PARAMS(2);
                uint32_t value = *reinterpret_cast<uint32_t *>(&stack[sp - 1]);
                value = std::rotr(value, stack[sp - 2]);
                stack[sp - 1] = *reinterpret_cast<int32_t *>(&value);
                break;
            }

            case opcode_for("arpushadd"): ARPUSH(+)
                break;
            case opcode_for("arpopadd"): ARPOP(+)
                break;
            case opcode_for("arpushsub"): ARPUSH(-)
                break;
            case opcode_for("arpopsub"): ARPOP(-)
                break;
            case opcode_for("arpushmul"): ARPUSH(*)
                break;
            case opcode_for("arpopmul"): ARPOP(*)
                break;
            case opcode_for("arpushdiv"): ARPUSH(/)
                break;
            case opcode_for("arpopdiv"): ARPOP(/)
                break;
            case opcode_for("arpushmod"): ARPUSH(%)
                break;
            case opcode_for("arpopmod"): ARPOP(%)
                break;
            case opcode_for("arpushand"): ARPUSH(&)
                break;
            case opcode_for("arpopand"): ARPOP(&)
                break;
            case opcode_for("arpushor"): ARPUSH(|)
                break;
            case opcode_for("arpopor"): ARPOP(|)
                break;

            case opcode_for("pushm"):
                PUSHES_VALUES(1);
                swap(stack[sp], memory.at(operand));
                sp += 1;
                break;
            case opcode_for("popm"):
                REQUIRES_PARAMS(1);
                sp -= 1;
                swap(stack[sp], memory.at(operand));
                clear(stack[sp], 0);
                break;

            case opcode_for("load"):
                PUSHES_VALUES(1);
                REQUIRES_PARAMS(1);
                swap(stack[sp], memory.at(stack[sp - 1] + operand));
                sp += 1;
                break;
            case opcode_for("store"):
                REQUIRES_PARAMS(2);
                sp -= 1;
                swap(stack[sp], memory.at(stack[sp - 1] + operand));
                clear(stack[sp], 0);
                break;

            case opcode_for("memswap"):
            case INVERSE(opcode_for("memswap")):
                REQUIRES_PARAMS(2);
                swap(memory.at(stack[sp - 1]), memory.at(stack[sp - 2]));
                break;

            case opcode_for("xorhc"):
            case INVERSE(opcode_for("xorhc")):
                REQUIRES_PARAMS(1);
                // Don't use operand here, since it is sign-extended and we want the raw bits.
                stack[sp - 1] ^= (instruction & OPCODE_WIDTH_MASK) << (OPERAND_WIDTH - 1);
                break;

            default:
                throw illegal_instruction(instruction, opcode);
        }
    }


    void VM::step() {
        this->counter++;
        this->step_instr();
        this->step_pc();
    }

    void VM::run() {
        do {
            this->step();
        } while (this->running);
    }

    VM::VM(const std::vector<int32_t> &program,
           const MemoryLayout &memory_layout,
           const size_t memory_size,
           const size_t stack_size,
           const int32_t pc) :
            dir(Forward), pc(pc), br(0), sp(0), fp(0),
            memory(memory_size), stack(stack_size),
            running(false), counter(0), program(program) {
        for (const auto &[address, value]: memory_layout) {
            memory.at(address) = value;
        }
    }
}
