#include <stdlib.h>
#include <stdio.h>

#include "decompiler.h"

#include "utils.h"
#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "pretty_printer.h"
#include "graph.h"
#include "SSA.h"


// TODO: delete
#include "graph_utils.h"

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
               CFG                          <- done
                │
                v
            SSA (often)                     <- TODO
                │
                v
          Optimisations                     <- skipped
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
int decompile(char* file_path) {
    char* asmfile = read_asm_file(file_path);

    char* backup_text = asmfile;
    int nb_instructions;

    // TODO remove white space and carriage return
    TokenList* tokenList = lexer(&asmfile, &nb_instructions);

    Asm* program = parser(tokenList, nb_instructions);

    // Visitor visitor;
    // visit_program(&visitor, program, stdout);
    
    Cfg* cfg = asm_to_cfg(program);

    graph_to_graphviz(cfg, "graph.dot", "graph.png");
    
    dominator_tree(cfg);
    
    // cfg_to_SSA_form(cfg);

    // simplify_cfg(cfg);

    // graph_to_graphviz(cfg, "graph_simplified.dot", "graph_simplified.png");

    free_asm(program);
    free_cfg(cfg);
    free_tokens(tokenList);
	free(backup_text);

    return 0;
}