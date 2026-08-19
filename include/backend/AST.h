#ifndef H_AST
#define H_AST

#include "backend/CFG.h"

// -------------------------------------- AsmCfg structure -----------------------------------------
typedef enum LoopKind
{
    LOOP_WHILE,
    LOOP_DOWHILE,
    LOOP_ENDLESS
} LoopType;

typedef enum AstNodeKind
{
    AST_NODE,
    AST_SEQUENCE,
    AST_IF,
    AST_SWITCH,
    AST_LOOP,
} AstNodeKind;

typedef struct ASTSequence
{
    Node* nodes;
} ASTSequence;

typedef struct ASTIf
{
    ASMCondition* cond;
    Node* true_branch;
    Node* false_branch;
} ASTIfCondition;

typedef struct ASTLoop
{
    LoopKind kind;
    ASMCondition* cond;
    Node* body;
} ASTLoop;

typedef struct ASTCase
{
    int cond;
    Node* node;
} ASTCase;

typedef struct ASTSwitch
{
    int var;
    ASTCase* cases;
    Node* default_node;
} ASTSwitch;

typedef struct ASTNode
{
    AstNodeKind kind;

    union
    {
        Node* node;
        ASTSequence* sequence;
        ASTIf* ifcondition;
        ASTSwitch* switchcondition;
        ASTLoop* loop;
    };
} ASTNode;

typedef struct Ast
{
    ASTNode* astnode;
} Ast;

#endif