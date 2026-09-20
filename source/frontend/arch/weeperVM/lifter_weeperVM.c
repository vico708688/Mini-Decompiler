#include <stdlib.h>
#include <stdio.h>

#include "frontend/arch/weeperVM/lifter_weeperVM.h"
#include "backend/CFG.h"
#include "backend/IR.h"

ASMOpcode opcode_to_ir(ASMOpcode opcode)
{
    switch (opcode)
    {
    case OPCODE_ADD:
        return ATM_ADD;
    case OPCODE_SUB:
        return ATM_SUB;
    case OPCODE_MUL:
        return ATM_MUL;
    case OPCODE_REM:
        return ATM_REM;
    case OPCODE_AND:
        return ATM_AND;
    case OPCODE_OR:
        return ATM_OR;
    case OPCODE_XOR:
        return ATM_XOR;
    case OPCODE_SHL:
        return ATM_SHL;
    case OPCODE_SHR:
        return ATM_SHR;
    case OPCODE_JNZ:
    case OPCODE_JA:
    case OPCODE_JE:
    case OPCODE_JL:
        return ATM_CONDJMP;
    case OPCODE_CMP:
        return ATM_EQUAL;
    case OPCODE_JMP:
        return ATM_JMP;
    case OPCODE_CALL:
        return ATM_CALL;

    default:
        fprintf(stderr, "Error lifting, the opcode number %d is not a valid opcode.\n", opcode);
        exit(1);
        break;
    }
}

IR *lifter_weeperVM(ASM *program)
{
    IR *ir = malloc(sizeof(IR));
    if (ir == NULL)
    {
        perror("malloc() IR failed\n");
        exit(EXIT_FAILURE);
    }

    ir->index = 0;
    ir->capacity = program->nb_instructions;
    ir->atomic_instructions = malloc(program->nb_instructions * sizeof(AtomicInstr));
    if (ir->atomic_instructions == NULL)
    {
        perror("malloc() Atoms failed\n");
        exit(EXIT_FAILURE);
    }

    /* lift every instructions */
    for (int i = 0; i < program->nb_instructions; i++)
    {
        // printf("%d\n", i);
        lift_instruction(ir, &program->instructions[i]);
    }

    // // resolve unresolved jumps
    // for (int offset = 0; offset < ir->index; offset++)
    // {
    //     AtomicInstr instr = ir->atomic_instructions[offset];
    //     AtomicInstrKind kind = instr.kind;

    //     if (kind == asm_instruction->opcode)
    //     {
    //         int jump_addr = instr.jump->true_branch->offset;
    //         int index = find_instruction_offset(ir, jump_addr);

    //         if (index != -1)
    //         {
    //             instr.jump->jump_resolved = true;

    //             int offset_true_branch = instr.jump->true_branch->offset;
    //             free(instr.jump->true_branch);
    //             instr.jump->true_branch = &(ir->instructions[offset_true_branch]);

    //             if (instr.jump->condition->kind != COND_TRUE)
    //             {
    //                 int offset_false_branch = instr.jump->false_branch->offset;
    //                 free(instr.jump->false_branch);
    //                 instr.jump->false_branch = &(ir->instructions[offset_false_branch]);
    //             }
    //         }
    //         else
    //         {
    //             fprintf(stderr, "[-] Unable to resolve the jump address, TODO: move this in SSA :)\n");
    //             exit(1);
    //         }
    //     }
    // }

    return ir;
}

// ------------------------------------------- void lifting ----------------------------------------------
LiftFunc find_lifting_op_instruction(ASMOpcode opcode, Lift_DispatchTable *table)
{
    for (int i = 0; i < OPCODE_COUNT; i++)
    {
        if (table[i].opcode == opcode)
        {
            return table[i].func;
        }
    }

    return NULL;
}

LiftJmpFunc find_lifting_jmp_instruction(ASMOpcode opcode, Lift_jmp_DispatchTable *table)
{
    for (int i = 0; i < OPCODE_COUNT; i++)
    {
        if (table[i].opcode == opcode)
        {
            return table[i].func;
        }
    }

    return NULL;
}

void lift_instruction(IR *ir, ASMInstruction *asm_instruction)
{
    Lift_DispatchTable lifter_op_table[OPCODE_COUNT] = {
        {OPCODE_MOV, lift_mov},
        {OPCODE_MOVB, lift_mov},
        {OPCODE_CMP, lift_cmp}, // Do I need to lift cmp ?

        {OPCODE_ADD, lift_operation},
        {OPCODE_SUB, lift_operation},
        {OPCODE_XOR, lift_operation},
        {OPCODE_AND, lift_operation},
        {OPCODE_SHL, lift_operation},
        {OPCODE_SHR, lift_operation},
        {OPCODE_OR, lift_operation},
        {OPCODE_REM, lift_operation},
    };

    Lift_jmp_DispatchTable lifter_jmp_table[OPCODE_COUNT] = {
        {OPCODE_JNZ, lift_jmp},
        {OPCODE_JA, lift_jmp},
        {OPCODE_JE, lift_jmp},
        {OPCODE_JL, lift_jmp},
        {OPCODE_JMP, lift_jmp},
    };

    LiftFunc lifter_op = find_lifting_op_instruction(asm_instruction->opcode, lifter_op_table);

    if (lifter_op != NULL)
    {
        lifter_op(ir, asm_instruction);
    }
    else
    {
        LiftJmpFunc lifter_jmp = find_lifting_jmp_instruction(asm_instruction->opcode, lifter_jmp_table);

        if (lifter_jmp != NULL)
        {
            lifter_jmp(ir, asm_instruction);
        }
        // TODO: add lift_call
    }

    ir->atomic_instructions[ir->index].offset = ir->index;
}

void lift_cmp(IR *ir, ASMInstruction *asm_instruction)
{
    AtomicInstr atom = {0};

    ValueId input0 = operand_to_id(asm_instruction->operation.arg0, &ir->atomic_instructions[ir->index], SIZE_WORD);
    ValueId input1 = operand_to_id(asm_instruction->operation.arg1, &ir->atomic_instructions[ir->index], SIZE_WORD);

    atom.kind = opcode_to_ir(asm_instruction->opcode);
    atom.equal.input0 = input0;
    atom.equal.input1 = input1;

    add_atomic_operation(ir, atom);
}

void lift_mov(IR *ir, ASMInstruction *asm_instruction)
{
    ASMOperand *dst_op = asm_instruction->operation.arg0;
    ASMOperand *src_op = asm_instruction->operation.arg1;

    AtomicInstr atom = {0};
    AtomicInstr *ir_instruction = &ir->atomic_instructions[ir->index];

    // determine if this is a Store or a Load based on operand kinds
    if (dst_op->kind == OPE_MEM)
    {
        // STORE (ex: movb mem[reg[]], reg[])

        // the pointer is what is INSIDE the mem[...]
        atom.kind = ATM_STORE;
        atom.store.pointer = operand_to_id(dst_op->index, ir_instruction, SIZE_WORD);
        atom.store.source = operand_to_id(src_op, ir_instruction, SIZE_WORD);
    }
    else if (src_op->kind == OPE_MEM)
    {
        // LOAD (ex: reg = mem[reg[]])

        atom.kind = ATM_LOAD;
        atom.load.pointer = operand_to_id(src_op->index, ir_instruction, SIZE_WORD);
        atom.load.destination = operand_to_id(dst_op, ir_instruction, SIZE_WORD);
    }
    else
    {
        // COPY (ex: movb reg[], reg[] OR movb reg[], CONST)

        atom.kind = ATM_COPY;
        atom.copy.source = operand_to_id(src_op, ir_instruction, SIZE_BYTE);
        atom.copy.destination = operand_to_id(dst_op, ir_instruction, SIZE_BYTE);
    }

    add_atomic_operation(ir, atom);
}

void add_atomic_operation(IR *ir, AtomicInstr atom)
{
    if (ir->index >= ir->capacity)
    {
        ir->capacity *= 2;

        AtomicInstr *new_atoms = realloc(ir->atomic_instructions, ir->capacity * sizeof(AtomicInstr));

        if (new_atoms == NULL)
        {
            fprintf(stderr, "realloc() atomic_instructions failed\n");
            exit(EXIT_FAILURE);
        }

        ir->atomic_instructions = new_atoms;
    }

    ir->atomic_instructions[ir->index++] = atom;
}

ValueId operand_to_id(ASMOperand *operand, AtomicInstr *ir_instruction, Size type_size)
{
    ValueId valueId = {0};
    Address address = {0};

    switch (operand->kind)
    {
    case OPE_IMM:
        address.space = SPACE_CONST;
        address.offset = operand->value;
        break;

    case OPE_REG:
        address.space = SPACE_REG;
        if (operand->index != NULL)
        {
            address.offset = operand->index->value;
        }
        else
        {
            fprintf(stderr, "Error: The register doesn't have an index\n");
            exit(1);
        }
        break;

    case OPE_MEM:
        fprintf(stderr, "Fatal error : OPE_MEM given to operand_to_id !\n");
        exit(1);
        break;

    default:
        break;
    }

    valueId.def = ir_instruction;
    valueId.address = address;
    valueId.size = type_size;
    valueId.version = 0;

    return valueId;
}

/**
 * Example:
 *
 * Input:
 *  ADD RAX, 10
 *
 * Output:
 *  input0 = ID(RAX)
 *  input1 = ID(CONST_10)
 *  output = ID(RAX + CONST_10) --> not for now
 */
void lift_operation(IR *ir, ASMInstruction *asm_instruction)
{
    AtomicInstr atom = {0};

    ASMOperand *arg0 = asm_instruction->operation.arg0;
    ASMOperand *arg1 = asm_instruction->operation.arg1;

    ValueId input0 = operand_to_id(arg0, &ir->atomic_instructions[ir->index], SIZE_WORD);
    ValueId input1 = operand_to_id(arg1, &ir->atomic_instructions[ir->index], SIZE_WORD);
    atom.kind = opcode_to_ir(asm_instruction->opcode);

    switch (asm_instruction->opcode)
    {
    case OPCODE_ADD:
        atom.add.input0 = input0;
        atom.add.input1 = input1;
        break;
    case OPCODE_SUB:
        atom.sub.input0 = input0;
        atom.sub.input1 = input1;
        break;
    case OPCODE_MUL:
        atom.mul.input0 = input0;
        atom.mul.input1 = input1;
        break;
    case OPCODE_REM:
        atom.rem.input0 = input0;
        atom.rem.input1 = input1;
        break;
    case OPCODE_AND:
        atom.and.input0 = input0;
        atom.and.input1 = input1;
        break;
    case OPCODE_OR:
        atom.or.input0 = input0;
        atom.or.input1 = input1;
        break;
    case OPCODE_XOR:
        atom.xor.input0 = input0;
        atom.xor.input1 = input1;
        break;
    case OPCODE_SHL:
        atom.shl.input0 = input0;
        atom.shl.input1 = input1;
        break;
    case OPCODE_SHR:
        atom.shr.input0 = input0;
        atom.shr.input1 = input1;
        break;
    default:
        fprintf(stderr, "Don't forget to add the opcode in lift_operation ;)\n");
        exit(1);
        break;
    }

    add_atomic_operation(ir, atom);
}

void lift_jmp(IR *ir, ASMInstruction *asm_instruction)
{
    AtomicInstr atom = {0};

    ASMOperand *arg0 = asm_instruction->operation.arg0;
    ValueId input0 = operand_to_id(arg0, &ir->atomic_instructions[ir->index], SIZE_WORD);

    atom.kind = opcode_to_ir(asm_instruction->opcode);
    // if (asm_instruction->operation.arg1 != NULL)
    // {
    //     ASMOperand *arg1 = asm_instruction->operation.arg1;
    //     ValueId input1 = operand_to_id(arg1);
    //     input1.def = &ir->atomic_instructions[ir->index];
    // }

    switch (asm_instruction->opcode)
    {
    case OPCODE_JNZ:
    case OPCODE_JA:
    case OPCODE_JE:
    case OPCODE_JL:
        atom.cond_jmp.jmp_addr = input0;
        break;
    case OPCODE_JMP:
        atom.jmp.jmp_addr = input0;
        break;
    default:
        fprintf(stderr, "And don't forget to implement in lift_jmp :)\n");
        exit(1);
        break;
    }

    add_atomic_operation(ir, atom);
}