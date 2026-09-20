#ifndef H_LIFTER_WEEPERVM
#define H_LIFTER_WEEPERVM

#include "frontend/arch/weeperVM/ASM_weeperVM.h"
#include "backend/IR.h"

IR *lifter_weeperVM(ASM *program);

void lift_instruction(IR *ir, ASMInstruction *asm_instruction);
void lift_operation(IR *ir, ASMInstruction *asm_instruction);
void lift_movb(IR *ir, ASMInstruction *asm_instruction);
void lift_mov(IR *ir, ASMInstruction *asm_instruction);
void lift_jmp(IR *ir, ASMInstruction *asm_instruction);
void lift_cmp(IR *ir, ASMInstruction *asm_instruction);

void add_atomic_operation(IR *ir, AtomicInstr atom);
ValueId operand_to_id(ASMOperand *operand, AtomicInstr *ir_instruction, Size type_size);

LiftJmpFunc find_lifting_jmp_instruction(ASMOpcode opcode, Lift_jmp_DispatchTable *table);
LiftFunc find_lifting_op_instruction(ASMOpcode opcode, Lift_DispatchTable *table);

#endif