#ifndef LEXER_H
#define LEXER_H

#include "../tokenizer/tokenizer.h"

typedef enum { L_IDENT, L_STRING, L_KEYWORD, L_SYMBOL, L_EOF } LexType;

typedef struct {
    LexType type;
    Token token;
} Lex;

typedef struct {
    Lex *items;
    size_t count;
} LexList;

LexList lex(TokenList tokens);
void free_lex(LexList *ll);

#endif
