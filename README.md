# Kcats – Backwards on the Stack


This project is part of my master's degree at the *University of Applied Sciences THM* in Gießen, Germany. Its goal is to explore the design space of a reversible stack-based machine. The machine and its virtual machine implementation have three main design goals:
1. **Clean** – the instruction set should be clean, i.e. it does not generate garbage during execution and is therefore fully reversible.
2. **Simple** – the instruction set should not contain unnecessary complexity. Following this design goal, a RISC like instruction set is used for the machine.
3. **Performance-Oriented** – Execution should be fast: Decoding and executing instructions requires minimal computational power, yielding a high execution rate.

## Build & Execution

To build the virtual machine implementation that is provided with this project, you need to run the [build.sh build script bundled in this repository](build.sh).
It uses CMake and Ninja to build a C++ based executable.

```sh
./build.sh
```

If no errors or warnings are generated during the build process, a new file shuold be generated as `target/stackmachine`.
This is the executable virtual machine.

An additional **performance optimized** build mode is available.
If the virtual machine is build in this mode, it will skip some runtime tests in favor of performance.
In this mode, instruction operands, stack overflows and stack underflows are not explicitly checked.
Additionally, no checks are performed when clearing a stack slot.
This means, that cases where a program might become irreversible are not explicitly checked, for example if a wrong constant is popped from the stack or a local variable is not cleared at the end of a procedure.
In a correct program, these cases should not occur, making it viable to squeeze out additional performance in these cases.
Even in this mode, a program can stil fail, if it performs illegal arithmetic instructions or accesses protected memory regions.

To build the executable in the performance-optimized mode, the `UNSAFE_OPERATIONS` preprocessor macro must be defined.
This can be achieved by enabling the equally-named CMake option:

```sh
./build.sh -DUNSAFE_OPERATIONS=ON
```

To force a rebuild of the executable, the `target` directory can be deleted.

## General Design


## Project Name

The project name is a word play on the word "stack".
Since this machine supports backwards execution, it only seems natural to read the word "stack" backwards.
The resulting word "kcats" contains the cat that is used as an icon for this project.

---

Copyright (C) 2022, Niklas Deworetzki
