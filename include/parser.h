#ifndef H_PARSER
#define H_PARSER

#include "token.h"

#include <stdbool.h>

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
    bool jump_resolved; // TODO: change into 2 fields of int (true_offset and false_offset) -> cleaner
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

typedef Instruction (*ParseFunc)(TokenList*);
typedef struct Parse_DispatchTable
{
    enum Token_t tokenType;
    ParseFunc func;
} Parse_DispatchTable;

typedef Instruction (*ParseJmpFunc)(TokenList*, int);
typedef struct Parse_jmp_DispatchTable
{
    enum Token_t tokenType;
    ParseJmpFunc func;
} Parse_jmp_DispatchTable;

Asm* parser(TokenList* tokenList, int nb_instructions);

Token show_next_token(TokenList* tokenList);
Token accept_token(TokenList* tokenList);
Token expect_token(TokenList* tokenList, enum Token_t tokenType);

int find_instruction_offset(Asm* program, int offset);

// ------------------------------------------- Asm parsing ----------------------------------------------
Asm* parse_asm(TokenList* tokenList, int nb_instructions);

// ------------------------------------------- Instruction parsing ----------------------------------------------
Instruction parse_instruction(TokenList* tokenList, int offset);
ParseFunc find_parsing_instruction(enum Token_t tokenType, Parse_DispatchTable* table);
ParseJmpFunc find_parsing_jmp_instruction(enum Token_t tokenType, Parse_jmp_DispatchTable* table);

Operand* parse_operand(TokenList* tokenList);
Operation* parse_operation(TokenList* tokenList);
ExprInt* parse_int(TokenList* tokenList);
Dst* parse_dst(TokenList* tokenList);
Src* parse_src(TokenList* tokenList);

Instruction parse_mov(TokenList* tokenList);
Instruction parse_movb(TokenList* tokenList);
Instruction parse_op(TokenList* tokenList);
Instruction parse_cmp(TokenList* tokenList);
Instruction parse_call(TokenList* tokenList);
Instruction parse_jmp(TokenList* tokenList, int offset);

#endif