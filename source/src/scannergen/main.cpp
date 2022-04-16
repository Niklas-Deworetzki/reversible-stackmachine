
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
    uint32_t index = 0;
    for (const auto &item: KNOWN_INSTRUCTIONS) {
        if (strcmp(item.fw_mnemonic, item.bw_mnemonic) == 0) {
            print_scanner_line(item.fw_mnemonic, index, true);
        } else {
            print_scanner_line(item.fw_mnemonic, index, true);
            print_scanner_line(item.bw_mnemonic, index, false);
        }

        index++;
    }
    fprintf(stderr, "A total of %lu lexical rules have been generated\n",
            sizeof(KNOWN_INSTRUCTIONS) / sizeof(*KNOWN_INSTRUCTIONS));
}
