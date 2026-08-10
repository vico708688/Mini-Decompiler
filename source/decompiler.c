#include <stdlib.h>
#include <stdio.h>

#include "decompiler.h"

#include "utils.h"
#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "pretty_printer.h"
#include "graph.h"

/**
 * It assumes as input an assembler code file from a disassembler that 
 * already converted binary opcodes to mnemonic representation and added
 * location labels for jumps inside a sub function.
 * 
 *        VM Instructions 
                │
                v
             Decoding                       <- done
                │
                v
                IR
      (architecture independent)            <- skipped
                │
                v
               CFG
                │
                v
            SSA (often)
                │
                v
          Optimisations
 (constant folding, propagation...)
                │
                v
        CFG structuration
         (No More Gotos)
                │
                v
               AST
                │
                v
        C Code / pseudo-code
 */
int decompile(char* file_path) {
    char* asmfile = read_asm_file(file_path);

    char* backup_text = asmfile;
    int nb_instructions;

    TokenList tokenList = lexer(&asmfile, &nb_instructions);

    Asm* program = parser(&tokenList, nb_instructions);

    // Visitor visitor;

    // visit_program(&visitor, program);
    

    Cfg* cfg = asm_to_cfg(program);

    // graph_to_graphviz(cfg, "graph.dot", "graph.png");

    // simplify_cfg(cfg);

    // graph_to_graphviz(cfg, "graph_simplified.dot", "graph_simplified.png");

    freeTokens(&tokenList);
	free(backup_text);

    return 0;
}