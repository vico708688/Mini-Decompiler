#include <stdlib.h>
#include <stdio.h>

#include "ccg.h"

#include "utils/utils.h"
#include "frontend/token.h"
#include "frontend/lexer.h"
#include "frontend/parser.h"
#include "backend/graph.h"
#include "backend/SSA.h"
#include "backend/cf_structuring.h"
#include "utils/printCFG.h"

// TODO : remove
#include "utils/graph_utils.h"


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
                IR                          <- done
                │
                v
               CFG                          <- done
                │
                v
               SSA                          <- TODO
                │
                v
          Optimisations                     <- 
 (constant folding, propagation...)
                │
                v
        CFG structuration                   <- 
         (No More Gotos)
                │
                v
               AST
                │
                v
        C Code / pseudo-code
 */
int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s asm_file.txt\n", argv[0]);
        exit(0);
    }

    int ret = decompile(argv[1]);
    return ret;
}

int decompile(char* file_path) {
    char* asmfile = read_asm_file(file_path);

    char* backup_text = asmfile;
    int nb_instructions;

    // TODO remove white space and carriage return
    TokenList* tokenList = lexer(&asmfile, &nb_instructions);

    Asm* program = parser(tokenList, nb_instructions);
    
    Cfg* cfg = asm_to_cfg(program);

    cfg_to_SSA_form(cfg);
    
    // compute_dominance_frontier(cfg);
    // graph_to_graphviz(cfg, "graph.dot", "graph.png");
    
    free_asm(program);
    free_cfg(cfg);
    free_tokens(tokenList);
	free(backup_text);

    return 0;
}