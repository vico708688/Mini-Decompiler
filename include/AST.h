#ifndef H_AST
#define H_AST

#include <stdbool.h>
// #include "graph.h"
// #include "lifter.h"

typedef struct Operand Operand;
typedef struct Operation Operation;
typedef struct ExprReg ExprReg;
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

typedef enum AssignmentKind
{
    ASSIGN_REG,
    ASSIGN_MEM,
    ASSIGN_INT
} AssignmentKind;

typedef enum ConditionKind
{
    COND_LESS,
    COND_ABOVE,
    COND_EQ,
} ConditionKind;

typedef enum OperationKind
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
} OperationKind;

typedef enum OperandKind
{
    OPERAND_INT,
    OPERAND_REG,
    OPERAND_MEM
} OperandKind;

typedef enum DstKind
{
    DST_REG,
    DST_MEM,
    DST_FLAG
} DstKind;

typedef enum SrcKind
{
    SRC_INT,
    SRC_REG,
    SRC_MEM,
    SRC_FLAG
} SrcKind;


// ------------------- BASIC STRUCTS ----------------------
typedef struct ExprInt
{
    int value;
} ExprInt;

typedef struct ExprReg
{
    Operand* index;
} ExprReg;

typedef struct ExprMem
{
    Operand* index;
} ExprMem;

typedef struct ExprFlag
{
    int index;
} ExprFlag;



// ------------------- BASIC BLOCKS OF INSTRUCTION ----------------------
typedef struct Dst
{
    DstKind kind;
    
    union
    {
        ExprReg* reg;
        ExprMem* mem;
        ExprFlag* flag;
    };
} Dst;

typedef struct Src
{
    SrcKind kind;
    
    union
    {
        ExprInt* integer;
        ExprReg* reg;
        ExprMem* mem;
        ExprFlag* flag;
    };
} Src;

typedef struct Operand
{
    OperandKind kind;
    
    union
    {
        ExprInt* integer;
        ExprReg* reg;
        ExprMem* mem;
    };
} Operand;

typedef struct Condition
{
    Operation* lcond;
    ConditionKind kind;
    Operation* rcond;
} Condition;

// ------------------- BASIC IR INSTRUCTIONS ----------------------
typedef struct Operation
{
    Dst* dst;
    OperationKind operator;
    Src* src;
} Operation;

typedef struct Load
{
    Dst* dst;
    Src* src;
} Load;

typedef struct Store
{
    Dst* dst;
    Src* src;
} Store;

typedef struct Comparison
{
    Dst* dst;
    Src* src;
} Comparison;

typedef struct Call
{
    char* name;
} Call;

typedef struct Jump
{
    Condition* condition;
    bool jump_resolved;
    Instruction* true_branch;
    Instruction* false_branch;
} Jump;



// ------------------- INSTRUCTION ----------------------
/**
 * Load(dst, src)
 * Store(dst, src)
 * 
 * Add  (dst, src) -
 * Sub  (dst, src) |
 * Mul  (dst, src) |
 * Div  (dst, src) |
 * And  (dst, src) | ---> Operation(dst, src)
 * Or   (dst, src) |
 * Xor  (dst, src) -
 * 
 * Cmp  (dst, src) --, to add in jump
 *                   v
 * Jump(cond, true, false)
 */
typedef struct Instruction
{
    InstructionKind kind;
    int offset;

    union {
        Load* load;
        Store* store;
        Operation* operation;
        Comparison* comparison;
        Jump* jump;
        Call* call;
    };
} Instruction;

typedef struct Asm
{
    Instruction* instructions;
    int nb_instructions;
    int instr_idx;
} Asm;


// // -------------------------------------- AsmCfg structure -----------------------------------------
// typedef struct {
//     Stmt* decl;
// } AsmCfg;

// typedef enum LoopType {
//     LOOP_WHILE,
//     LOOP_DOWHILE,
//     LOOP_ENDLESS
// } LoopType;

// typedef struct Sequence {
//     Node* nodes;
// } Sequence;

// typedef struct If {
//     ExprOperator cond;
//     Node* true_branch;
//     Node* false_branch;
// } If;

// typedef struct Loop {
//     LoopType kind;
//     ExprOperator cond;
//     Node* body;
// } Loop;

// typedef struct Case {
//     int cond;
//     Node* node;
// } Case;

// typedef struct Switch {
//     int var;
//     Case* cases;
//     Node* default_node;
// } Switch;

#endif