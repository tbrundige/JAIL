#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"

typedef enum {
    AST_PROGRAM,
    AST_VAR_DECL,
    AST_FUNC_DEF,
    AST_CALL,
    AST_STRING,
    AST_IDENT,
    AST_CONCAT
} ASTType;

typedef struct AST {
    ASTType type;
    char *name;
    char **params;
    size_t param_count;
    struct AST **children;
    size_t child_count;
    char *str;
} AST;

AST *parse(LexList ll);
void free_ast(AST *a);

#endif
