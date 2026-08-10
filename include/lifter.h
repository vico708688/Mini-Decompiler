// #ifndef H_LIFTER
// #define H_LIFTER

// #include "parser.h"

// // ------------------- LIFTER ----------------------
// typedef struct Lifter
// {
//     int expr_idx;
//     Expr** expr;
// } Lifter;

// typedef void (*LiftFunc)(Instruction_txt*, Lifter*);

// typedef struct
// {
//     char *mnemonic;
//     LiftFunc func;
// } LiftEntry;

// // ------------------- FUNCTION PROTOTYPES ----------------------
// int lift(Lifter* lifter, Instruction* instructions, int nb_instructions);
// bool lift_instruction(Instruction* instructions, Lifter* lifter);

// void lift_mov(Instruction_txt* instr, Lifter* lifter);
// void lift_add(Instruction_txt* instr, Lifter* lifter);

// #endif