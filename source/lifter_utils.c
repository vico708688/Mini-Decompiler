// #include <stdio.h>
// #include <stdlib.h>

// #include "lifter.h"
// #include "lifter_utils.h"

// void print_operand(char* output, Operand operand)
// {
//     OperandKind operand_type = operand.kind;

//     if (operand_type == OPERAND_INT)
//     {
//         sprintf(output, "%d", operand.integer->value);
//     }
//     else if (operand_type == OPERAND_REG)
//     {
//         sprintf(output, "reg[%d]", operand.reg->index.value);
//     }
//     else if (operand_type == OPERAND_MEM)
//     {
//         sprintf(output, "mem[%d]", operand.mem->index.value);
//     }
//     else if (operand_type == OPERAND_OPERAND)
//     {
//         print_operand(output, *(operand.operand));
//         sprintf(output, "mem[%s]", output);
//     }
// }

// void print_assign(Assignment assign) {
//     char dst_txt[64];
//     char src_txt[64];

//     Operand dst = *(assign.dst);
//     Operand src = *(assign.src);

//     print_operand(dst_txt, dst);
//     print_operand(src_txt, src);
    
//     printf("mov %s, %s\n", dst_txt, src_txt);
// }

// void print_operation(Operation operator) {
//     return;
// }

// void print_expr(Expr expr) {
//     printf("printing expr");
//     if (expr.kind == EXPR_ASSIGN)
//     {
//         print_assign(*(expr.assignment));
//     }
//     else
//     {
//         print_operation(*(expr.operation));
//     }
// }