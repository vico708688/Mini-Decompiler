#ifndef H_ASM_WEEPERVM
#define H_ASM_WEEPERVM

#include <stdbool.h>

typedef struct ASMOperand ASMOperand;
typedef struct ASMInstruction ASMInstruction;
typedef struct IR IR;

typedef enum ASMOpcode
{
    OPCODE_MOV,
    OPCODE_ADD,
    OPCODE_SUB,
    OPCODE_MUL,
    OPCODE_REM,
    OPCODE_AND,
    OPCODE_OR,
    OPCODE_XOR,
    OPCODE_SHL,
    OPCODE_SHR,
    OPCODE_JNZ,
    OPCODE_JA,
    OPCODE_JE,
    OPCODE_JL,
    OPCODE_JMP,
    OPCODE_CMP,
    OPCODE_MOVB,
    OPCODE_CALL,

    OPCODE_COUNT,
} ASMOpcode;

typedef void (*LiftFunc)(IR *, ASMInstruction *);
typedef struct Lift_DispatchTable
{
    ASMOpcode opcode;
    LiftFunc func;
} Lift_DispatchTable;

typedef void (*LiftJmpFunc)(IR *, ASMInstruction *);
typedef struct Lift_jmp_DispatchTable
{
    ASMOpcode opcode;
    LiftJmpFunc func;
} Lift_jmp_DispatchTable;

typedef enum ASMOperandKind
{
    OPE_IMM,
    OPE_REG,
    OPE_MEM,
    OPE_FLAG,
} ASMOperandKind;

typedef enum CallName
{
    CALL_PUTCHAR,
    CALL_READ,
} CallName;

typedef struct ASMOperand
{
    ASMOperandKind kind;

    union
    {
        ASMOperand *index;
        int value;
    };
} ASMOperand;

// ------------------- INSTRUCTION -------------------
typedef struct ASMInstruction
{
    ASMOpcode opcode;

    union
    {
        struct ASMOperation
        {
            ASMOperand *arg0;
            ASMOperand *arg1;
        } operation;

        CallName call_name;
    };

    int offset;

} ASMInstruction;

typedef struct ASM
{
    ASMInstruction *instructions;
    int nb_instructions;
    int instr_idx;
} ASM;

#endif