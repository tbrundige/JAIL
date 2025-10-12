#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stddef.h>

typedef enum {
    T_EOF,
    T_IDENT,
    T_STRING,
    T_NUMBER,
    T_KEYWORD,
    T_SYMBOL,
    T_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char *text;
    size_t len;
    int line;
} Token;

typedef struct {
    Token *tokens;
    size_t count;
    size_t capacity;
} TokenList;

TokenList tokenize(const char *src);
void free_tokens(TokenList *tl);

#endif
