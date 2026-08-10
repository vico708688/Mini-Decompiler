// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <stdbool.h>

// #include "lifter.h"
// #include "lifter_utils.h"
// #include "utils.h"
// #include "parser.h"

// Lifter* init_lifter(Lifter* lifter, int nb_expr) {    
//     lifter->expr = malloc(nb_expr * sizeof(Expr));
//     lifter->expr_idx = 0;

//     return lifter;
// }

// void parse_instruction(char* instruction, Instruction_txt* out) {
//     char** split_spaces;
//     char** split_jump;

//     split(&split_spaces, instruction, " ");

//     out->mn = split_spaces[0];
//     out->dst = split_spaces[1];
//     out->src = split_spaces[2];

//     if (strstr(out->mn, "jmp") || strstr(out->mn, "ja") || strstr(out->mn, "je") || strstr(out->mn, "jl"))
//     {
//         out->jmp = (int)strtol(split_jump[0], NULL, 10);
//     } else {
//         out->jmp = -1;
//     }
    
//     int n = split(&split_jump, instruction, "|");
//     if (n != 0)
//     {
//         out->cond = split_jump[1];
//     } else {
//         out->cond = NULL;
//     }
// }

// char* get_substring(char* string, char* delim1, char* delim2) {
//     char* return_string;

//     return_string = strstr(string, delim2);
    
//     if (return_string != NULL)
//     {
//         *return_string = '\0';
//     }

//     return_string = strstr(string, delim1);

//     return return_string + 1;
// }

// Expr lift_expr() {
//     Expr expr;

//     return expr;
// }

// Operand lift_operand(char* operand_txt) {
//     Operand operand;

//     char* end_p;

//     int operand_value = (int)strtol(operand_txt, &end_p, 10);

//     printf("operand value: %d\n", operand_value);

//     // if the operand is not an int
//     if (end_p != NULL)
//     {
//         if (strstr(operand_txt, "reg") != NULL) {
//             operand.kind = OPERAND_REG;

//             char* reg_idx = get_substring(operand_txt, "[", "]");
            
//             operand.reg->index.value = (int)strtol(reg_idx, &end_p, 10);
//         }
//     } else
//     {
//         fprintf(stderr, "Error destination: %s", operand_txt);
//         exit(1);
//     }

//     return operand;
// }

// void lift_mov(Instruction_txt* instr, Lifter* lifter) {
//     printf("mov\n");
//     Expr mov;
//     Assignment assign;

//     printf("%s, %s\n", instr->dst, instr->src);
    
//     (*assign.dst) = lift_operand(instr->dst);
//     (*assign.src) = lift_operand(instr->src);
    
//     mov.kind = EXPR_ASSIGN;
//     (*mov.assignment) = assign;

//     printf("before assign");
//     *lifter->expr[lifter->expr_idx++] = mov;
//     print_expr(*(lifter->expr[lifter->expr_idx++]));
// }

// void lift_add(Instruction_txt* instr, Lifter* lifter) {
//     printf("add\n");
    
// }

// LiftFunc find_lifter(char* mn, LiftEntry* table) {
//     for (int i = 0; table[i].mnemonic != NULL; i++)
//     {
//         if (strcmp(table[i].mnemonic, mn) == 0)
//         return table[i].func;
//     }
//     return NULL;
// }

// bool lift_instruction(Instruction* program, Lifter* lifter) {
//     Instruction_txt instr;
    
//     LiftEntry LiftEntry_table[] = {
//         {"mov", lift_mov},
//         {"add", lift_add}
//     };

//     // parse_instruction(instruction, &instr);
//     LiftFunc lift = find_lifter(instr.mn, LiftEntry_table);

//     if (lift != NULL)
//     {
//         // mnemonic in the entry table -> call lift_mov or lift_add, ...
//         lift(&instr, lifter);
//     } else
//     {
//         fprintf(stderr, "mnemonic %s unrecognized\n", instr.mn);
//     }

//     return 0;
// }

// int lift(Lifter* lifter, Instruction *program, int nb_instructions) {
//     init_lifter(lifter, nb_instructions);

//     for (int line_idx = 0; line_idx < nb_instructions; line_idx++)
//     {
//         printf("lifting instruction n°%d: ", line_idx + 1);
//         bool ret = lift_instruction(program, lifter);
//         printf("lifter expression index: %d\n", lifter->expr_idx);

//         if (ret)
//         {
//             fprintf(stderr, "Lift expr n°%d failed\n", line_idx);
//         }
//     }

//     return 0;
// }