/**
 * Program entry point for an auxiliary executable. When run, this executable
 * will generate lexical rules for every defined instruction, to be used in
 * the machine's parser. Rules are generated for the forward and backward
 * version of an instruction (unless they have the same mnemonic).
 */

#include <cstring>
#include <cstdio>
#include "syntax/instructions.h"

static void print_scanner_line(const char *name, uint32_t index, bool is_forward) {
    printf("\"%s\"%*c{ return symbol_instruction(%3d, %5s ); }\n",
           name,
           static_cast<int>(20 - strlen(name)), ' ',
           index, is_forward ? "true" : "false");
}

int main() {
    uint32_t index = 0, generated_rules = 0;
    for (const auto &item: KNOWN_INSTRUCTIONS) {
        if (strcmp(item.fw_mnemonic, item.bw_mnemonic) == 0) {
            print_scanner_line(item.fw_mnemonic, index, true);
            generated_rules++;
        } else {
            print_scanner_line(item.fw_mnemonic, index, true);
            print_scanner_line(item.bw_mnemonic, index, false);
            generated_rules += 2;
        }

        index++;
    }
    fprintf(stderr, "A total of %u lexical rules have been generated.\n",
            generated_rules);
}
