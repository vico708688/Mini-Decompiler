#ifndef H_ARCHITECTURE
#define H_ARCHITECTURE

typedef struct TokenList TokenList;
typedef struct ASM ASM;
typedef struct IR IR;

typedef struct Architecture
{
    char *name;
    TokenList *(*lexer_arch)(char **, int *);
    ASM *(*parser_arch)(TokenList *, int);
    IR *(*lifter_arch)(ASM *);

    void (*free_tokens)(TokenList *);
    void (*free_asm)(ASM *);
} Architecture;

#endif