#include <stdio.h>
#include <stdlib.h>

#include "decompiler.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s asm_file.txt\n", argv[0]);
        exit(0);
    }

    int ret = decompile(argv[1]);
    return ret;
}