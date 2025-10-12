#include "tokenizer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void push_token(TokenList *tl, Token t) {
    if (tl->count + 1 > tl->capacity) {
        tl->capacity = tl->capacity ? tl->capacity * 2 : 16;
        tl->tokens = realloc(tl->tokens, tl->capacity * sizeof(Token));
    }
    tl->tokens[tl->count++] = t;
}

static char *substr_dup(const char *s, size_t len) {
    char *r = malloc(len + 1);
    memcpy(r, s, len);
    r[len] = '\0';
    return r;
}

static int is_symbol_char(char c) {
    const char *syms = "=+(){},";
    return strchr(syms, c) != NULL;
}

TokenList tokenize(const char *src) {
    TokenList tl = {0};
    int line = 1;
    const char *p = src;
    while (*p) {
        if (*p == '\n') {
            line++;
            p++;
            continue;
        }
        if (isspace((unsigned char)*p)) {
            p++;
            continue;
        }

        if (*p == '/' && *(p + 1) == '/') {
            while (*p && *p != '\n')
                p++;
            continue;
        }

        if (*p == '"') {
            const char *start = ++p;
            while (*p && *p != '"') {
                if (*p == '\\' && *(p + 1))
                    p += 2;
                else
                    p++;
            }
            size_t len = p - start;
            char *txt = substr_dup(start, len);
            Token t = {T_STRING, txt, len, line};
            push_token(&tl, t);
            if (*p == '"')
                p++;
            continue;
        }

        if (isalpha((unsigned char)*p) || *p == '_') {
            const char *start = p;
            while (isalnum((unsigned char)*p) || *p == '_')
                p++;
            size_t len = p - start;
            char *txt = substr_dup(start, len);
            if (strcmp(txt, "var") == 0 || strcmp(txt, "func") == 0 ||
                strcmp(txt, "print") == 0) {
                Token t = {T_KEYWORD, txt, len, line};
                push_token(&tl, t);
            } else {
                Token t = {T_IDENT, txt, len, line};
                push_token(&tl, t);
            }
            continue;
        }

        if (is_symbol_char(*p)) {
            char *txt = substr_dup(p, 1);
            Token t = {T_SYMBOL, txt, 1, line};
            push_token(&tl, t);
            p++;
            continue;
        }

        char *txt = substr_dup(p, 1);
        Token t = {T_UNKNOWN, txt, 1, line};
        push_token(&tl, t);
        p++;
    }

    Token eof = {T_EOF, NULL, 0, line};
    push_token(&tl, eof);
    return tl;
}

void free_tokens(TokenList *tl) {
    if (!tl)
        return;
    for (size_t i = 0; i < tl->count; ++i) {
        free(tl->tokens[i].text);
    }
    free(tl->tokens);
    tl->tokens = NULL;
    tl->count = tl->capacity = 0;
}
