#ifndef H_ASM
#define H_ASM

typedef struct ASMOperand ASMOperand;
typedef struct ASMOperation ASMOperation;
typedef struct ASMReg ASMReg;
typedef struct Instruction Instruction;

// ------------------- KINDS ----------------------
typedef enum InstructionKind
{
    INSTR_LOAD,
    INSTR_STORE,
    INSTR_OP,
    INSTR_CMP,
    INSTR_JMP,
    INSTR_CALL,
} InstructionKind;

typedef enum ASMAssignmentKind
{
    ASSIGN_REG,
    ASSIGN_MEM,
    ASSIGN_INT
} ASMAssignmentKind;

typedef enum ASMConditionKind
{
    COND_TRUE,
    COND_LESS,
    COND_ABOVE,
    COND_EQ,
} ASMConditionKind;

typedef enum ASMOperationKind
{
    OP_ADD,
    OP_SUB,
    OP_XOR,
    OP_AND,
    OP_SHL,
    OP_SHR,
    OP_OR,
    OP_MOD,
    OP_EQ,
} ASMOperationKind;

typedef enum ASMOperandKind
{
    OPERAND_INT,
    OPERAND_REG,
    OPERAND_MEM
} ASMOperandKind;

typedef enum ASMDstKind
{
    DST_REG,
    DST_MEM,
    DST_FLAG
} ASMDstKind;

typedef enum ASMSrcKind
{
    SRC_INT,
    SRC_REG,
    SRC_MEM,
    SRC_FLAG
} ASMSrcKind;


// ------------------- BASIC STRUCTS: to delete ? ----------------------
typedef struct ASMInt
{
    int value;
} ASMInt;

typedef struct ASMReg
{
    ASMOperand* index;
} ASMReg;

typedef struct ASMMem
{
    ASMOperand* index;
} ASMMem;

typedef struct ASMFlag
{
    int index;
} ASMFlag;


// ------------------- BASIC BLOCKS OF INSTRUCTION ----------------------
typedef struct ASMDst
{
    ASMDstKind kind;
    
    union
    {
        ASMReg* reg;
        ASMMem* mem;
        ASMFlag* flag;
    };
} ASMDst;

typedef struct ASMSrc
{
    ASMSrcKind kind;
    
    union
    {
        ASMInt* integer;
        ASMReg* reg;
        ASMMem* mem;
        ASMFlag* flag;
    };
} ASMSrc;

typedef struct ASMOperand
{
    ASMOperandKind kind;
    
    union
    {
        ASMInt* integer;
        ASMReg* reg;
        ASMMem* mem;
    };
} ASMOperand;

typedef struct ASMCondition
{
    ASMOperation* lcond;
    ASMConditionKind kind;
    ASMOperation* rcond;
} ASMCondition;


// ------------------- BASIC IR INSTRUCTIONS ----------------------
typedef struct ASMOperation
{
    ASMDst* dst;
    ASMOperationKind operator;
    ASMSrc* src;
} ASMOperation;

typedef struct ASMLoad
{
    ASMDst* dst;
    ASMSrc* src;
} ASMLoad;

typedef struct ASMStore
{
    ASMDst* dst;
    ASMSrc* src;
} ASMStore;

typedef struct ASMComparison
{
    ASMDst* dst;
    ASMSrc* src;
} ASMComparison;

typedef struct ASMCall
{
    char* name;
} ASMCall;

typedef struct ASMJump
{
    ASMCondition* condition;
    bool jump_resolved; // TODO: change into 2 fields of int (true_offset and false_offset) -> cleaner
    Instruction* true_branch;
    Instruction* false_branch;
} ASMJump;


// ------------------- INSTRUCTION ----------------------
/**
 * ASMLoad(dst, src)
 * ASMStore(dst, src)
 * 
 * Add  (dst, src) -
 * Sub  (dst, src) |
 * Mul  (dst, src) |
 * Div  (dst, src) |
 * And  (dst, src) | ---> ASMOperation(dst, src)
 * Or   (dst, src) |
 * Xor  (dst, src) -
 * 
 * Cmp  (dst, src) --, to add in jump
 *                   v
 * ASMJump(cond, true, false)
 */
typedef struct Instruction
{
    InstructionKind kind;
    int offset;

    union {
        ASMLoad* load;
        ASMStore* store;
        ASMOperation* operation;
        ASMComparison* comparison;
        ASMJump* jump;
        ASMCall* call;
    };
} Instruction;

typedef struct Asm
{
    Instruction* instructions;
    int nb_instructions;
    int instr_idx;
} Asm;

#endif