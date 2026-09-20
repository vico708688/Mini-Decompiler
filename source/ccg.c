#include <stdlib.h>
#include <stdio.h>
#include <argp.h>
#include <stdbool.h>
#include <string.h>

#include "ccg.h"

#include "architecture.h"

#include "utils/utils.h"

#include "frontend/arch/weeperVM/lexer_weeperVM.h"
#include "frontend/arch/weeperVM/parser_weeperVM.h"
#include "frontend/arch/weeperVM/lifter_weeperVM.h"

Architecture architecture_weeperVM = {
    .name = "WeeperVM",
    .lexer_arch = lexer_weeperVM,
    .parser_arch = parser_weeperVM,
    .lifter_arch = lifter_weeperVM,
    .free_tokens = free_tokens_weeperVM,
    .free_asm = free_asm_weeperVM,
};

#include "backend/pretty_printer.h"
#include "backend/SSA.h"
// #include "backend/graph.h"
// #include "backend/cf_structuring.h"
// #include "utils/printCFG.h"

static char doc[] = "ccg: Tini decompiler for classical or custom architecture.";
static char args_doc[] = "[FILENAME]";
static struct argp_option options[] = {
    {"arch", 'a', "ARCH", 0, "Choose an architecture"},
    {"output", 'o', "FILE", 0, "Outputs to FILE instead of standard input"},
    {0}};

struct arguments
{
    char *args[1];
    char *arch;
    char *output_file;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;

    switch (key)
    {
    case 'a':
        arguments->arch = arg;
        break;
    case 'o':
        arguments->output_file = arg;
        break;
    case ARGP_KEY_ARG:
        if (state->arg_num >= 1)
            argp_usage(state);

        arguments->args[state->arg_num] = arg;

        break;

    case ARGP_KEY_END:
        if (state->arg_num < 1)
            argp_usage(state);
        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

/**
 * It assumes as input an assembler code file from a disassembler that
 * already converted binary opcodes to mnemonic representation and added
 * location labels for jumps inside a sub function.
 *
 *        VM Instructions
                │
                v
      Architecture selection                <- kinda done
                │
                v
             Decoding                       <- done
                │
                v
                IR                          <- TODO
                │
                v
               SSA                          <- TODO
                │
                v
               CFG                          <- done
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
               AST                          <-
                │
                v
        C Code / pseudo-code                <-
 */
int main(int argc, char *argv[])
{
    struct arguments arguments;
    arguments.arch = "WeeperVM";
    arguments.output_file = "-";

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    // printf("ARCH = %s\nOUTPUT_FILE = %s\nFILE = %s\n", arguments.arch, arguments.output_file, arguments.args[0]);

    int ret = decompile(arguments.arch, arguments.output_file, arguments.args[0]);
    return ret;
}

int decompile(char *arch, char *output, char *file_path)
{
    Architecture *architecture;
    /* Choice of the architecture -> replace with get_architecture(char* arch) */
    if (strcmp(arch, "WeeperVM") == 0)
    {
        architecture = &architecture_weeperVM;
    }

    /* frontend */
    char *asmfile = read_asm_file(file_path);
    char *backup_text = asmfile;
    
    int nb_instructions;
    
    TokenList *tokenList = architecture->lexer_arch(&asmfile, &nb_instructions);
    ASM *program = architecture->parser_arch(tokenList, nb_instructions);
    IR *ir = architecture->lifter_arch(program);

    Visitor v;
    visit_program(&v, ir, stdout);
    
    /* backend */
    ir_to_ssa(ir);

    // cfg building
    // CFG* cfg = ir_to_cfg(ir);

    // // compute_dominance_frontier(cfg);
    // // graph_to_graphviz(cfg, "graph.dot", "graph.png");

    // free_asm(program);
    // free_cfg(cfg);
    architecture->free_tokens(tokenList);
    architecture->free_asm(program);
    free(backup_text);

    return 0;
}