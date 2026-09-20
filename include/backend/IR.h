#ifndef H_IR
#define H_IR

#include <stdbool.h>

typedef struct ASMInstruction ASMInstruction;
typedef struct Reg Reg;
typedef struct AtomicInstr AtomicInstr;
typedef struct ValueId ValueId;

// ------------------- KINDS ----------------------
typedef enum AtomicInstrKind
{
    ATM_COPY,
    ATM_LOAD,
    ATM_STORE,
    ATM_JMP,
    ATM_CONDJMP,
    ATM_ADD,
    ATM_SUB,
    ATM_XOR,
    ATM_AND,
    ATM_OR,
    ATM_SHL,
    ATM_SHR,
    ATM_MUL,
    ATM_REM,
    ATM_EQUAL,
    ATM_LESS,
    ATM_LESSEQUAL,
    ATM_NOTEQUAL,
    ATM_CALL,
} AtomicInstrKind;

typedef enum SpaceId
{
    SPACE_CONST,
    SPACE_MEM,
    SPACE_REG,
} SpaceId;

typedef enum Size
{
    SIZE_BYTE,
    SIZE_WORD,
    SIZE_DWORD,
    SIZE_QWORD,
} Size;

typedef struct Address
{
    SpaceId space;
    int offset;
} Address;

typedef struct ValueId
{
    Address address;
    int size;
    int version;
    AtomicInstr *def;
} ValueId;

// ------------------- BASIC ATOMIC OPERATION ----------------------
typedef struct Copy
{
    ValueId source; // REG[] ou CONST
    ValueId destination; // REG[]
} Copy;

typedef struct Load
{
    ValueId pointer;     // MEM[] ou MEM[REG[]]
    ValueId destination; // REG[]
} Load;

typedef struct Store
{
    ValueId source;  // MEM[] ou MEM[REG[]]
    ValueId pointer; // REG[] ou CONST
} Store;

typedef struct Jmp
{
    ValueId jmp_addr;
} Jmp;

typedef struct CondJmp
{
    ValueId jmp_addr;
    bool taken;
} CondJmp;

typedef struct Add
{
    ValueId input0;
    ValueId input1;

    ValueId output;
} Add;

typedef struct Sub
{
    ValueId input0;
    ValueId input1;

    ValueId output;
} Sub;

typedef struct Xor
{
    ValueId input0;
    ValueId input1;

    ValueId output;
} Xor;

typedef struct And
{
    ValueId input0;
    ValueId input1;

    ValueId output;
} And;

typedef struct Or
{
    ValueId input0;
    ValueId input1;

    ValueId output;
} Or;

typedef struct Shl
{
    ValueId input0;
    ValueId input1;

    ValueId output;
} Shl;

typedef struct Shr
{
    ValueId input0;
    ValueId input1;

    ValueId output;
} Shr;

typedef struct Mul
{
    ValueId input0;
    ValueId input1;

    ValueId output;
} Mul;

typedef struct Rem
{
    ValueId input0;
    ValueId input1;

    ValueId output;
} Rem;

typedef struct Equal
{
    ValueId input0;
    ValueId input1;

    ValueId output;
} Equal;

typedef struct Less
{
    ValueId input0;
    ValueId input1;

    ValueId output;
} Less;

typedef struct LessEqual
{
    ValueId input0;
    ValueId input1;

    ValueId output;
} LessEqual;

typedef struct NotEqual
{
    ValueId input0;
    ValueId input1;

    ValueId output;
} NotEqual;

typedef struct Call
{
    ValueId destination;
    char *name;
} Call;

// ------------------- ATOMIC OPERATION STRUCT ----------------------
/* Inspired heavily by: https://ghidra.re/ghidra_docs/languages/html/pcodedescription.html */
typedef struct AtomicInstr
{
    AtomicInstrKind kind;
    int offset;

    union
    {
        Copy copy;
        Load load;
        Store store;
        Jmp jmp;
        CondJmp cond_jmp;
        Add add;
        Sub sub;
        Xor xor ;
        And and;
        Or or ;
        Shl shl;
        Shr shr;
        Mul mul;
        Rem rem;
        Equal equal;
        Less less;
        LessEqual less_equal;
        NotEqual not_equal;
        Call call;
    };

} AtomicInstr;

typedef struct IR
{
    AtomicInstr *atomic_instructions;
    int capacity;
    int index;
} IR;

#endif