#include "lexer.h"
#include <stdlib.h>
#include <string.h>

static Lex make_lex(LexType t, Token token) {
    Lex l;
    l.type = t;
    l.token.type = token.type;
    l.token.len = token.len;
    l.token.line = token.line;
    if (token.text)
        l.token.text = strdup(token.text);
    else
        l.token.text = NULL;
    return l;
}

LexList lex(TokenList tokens) {
    LexList ll = {0};
    if (tokens.count == 0)
        return ll;
    ll.items = malloc(tokens.count * sizeof(Lex));
    size_t idx = 0;
    for (size_t i = 0; i < tokens.count; ++i) {
        Token t = tokens.tokens[i];
        LexType lt;
        switch (t.type) {
        case T_IDENT:
            lt = L_IDENT;
            break;
        case T_STRING:
            lt = L_STRING;
            break;
        case T_KEYWORD:
            lt = L_KEYWORD;
            break;
        case T_SYMBOL:
            lt = L_SYMBOL;
            break;
        case T_EOF:
            lt = L_EOF;
            break;
        default:
            lt = L_IDENT;
            break;
        }
        ll.items[idx++] = make_lex(lt, t);
        if (t.type == T_EOF)
            break;
    }
    ll.count = idx;
    return ll;
}

void free_lex(LexList *ll) {
    if (!ll)
        return;
    for (size_t i = 0; i < ll->count; ++i)
        free(ll->items[i].token.text);
    free(ll->items);
    ll->items = NULL;
    ll->count = 0;
}
