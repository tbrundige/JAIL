#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    LexList *ll;
    size_t pos;
} PState;

static Lex *peek(PState *s) {
    return s->pos < s->ll->count ? &s->ll->items[s->pos] : NULL;
}
static Lex *advance(PState *s) {
    return s->pos < s->ll->count ? &s->ll->items[s->pos++] : NULL;
}

static AST *ast_new(ASTType t) {
    AST *a = calloc(1, sizeof(AST));
    a->type = t;
    return a;
}

static void ast_add_child(AST *parent, AST *child) {
    parent->children =
        realloc(parent->children, sizeof(AST *) * (parent->child_count + 1));
    parent->children[parent->child_count++] = child;
}

static char *dupstr(const char *s) { return s ? strdup(s) : NULL; }

static AST *parse_program(PState *s);
static AST *parse_statement(PState *s);
static AST *parse_expression(PState *s);

AST *parse(LexList ll) {
    PState s = {.ll = &ll, .pos = 0};
    return parse_program(&s);
}

static AST *parse_program(PState *s) {
    AST *prog = ast_new(AST_PROGRAM);
    while (peek(s) && peek(s)->type != L_EOF) {
        AST *st = parse_statement(s);
        if (st)
            ast_add_child(prog, st);
        else
            break;
    }
    return prog;
}

static int accept_symbol(PState *s, const char sym) {
    Lex *l = peek(s);
    if (l && l->type == L_SYMBOL && l->token.text && l->token.text[0] == sym) {
        advance(s);
        return 1;
    }
    return 0;
}

static int accept_keyword(PState *s, const char *kw) {
    Lex *l = peek(s);
    if (l && l->type == L_KEYWORD && l->token.text &&
        strcmp(l->token.text, kw) == 0) {
        advance(s);
        return 1;
    }
    return 0;
}

static AST *parse_var_decl(PState *s) {
    Lex *name = advance(s);
    if (!name || name->type != L_IDENT)
        return NULL;
    AST *node = ast_new(AST_VAR_DECL);
    node->name = dupstr(name->token.text);
    if (!accept_symbol(s, '='))
        return node;
    AST *expr = parse_expression(s);
    if (expr)
        ast_add_child(node, expr);
    return node;
}

static AST *parse_func_def(PState *s) {
    Lex *name = advance(s);
    if (!name || name->type != L_IDENT)
        return NULL;
    AST *node = ast_new(AST_FUNC_DEF);
    node->name = dupstr(name->token.text);
    if (!accept_symbol(s, '('))
        return node;
    while (peek(s) && peek(s)->type != L_EOF) {
        Lex *p = peek(s);
        if (p->type == L_IDENT) {
            p = advance(s);
            node->params =
                realloc(node->params, sizeof(char *) * (node->param_count + 1));
            node->params[node->param_count++] = dupstr(p->token.text);
            if (accept_symbol(s, ','))
                continue;
            else
                break;
        } else
            break;
    }
    accept_symbol(s, ')');
    accept_symbol(s, '{');
    while (peek(s) && !(peek(s)->type == L_SYMBOL && peek(s)->token.text &&
                        peek(s)->token.text[0] == '}')) {
        AST *st = parse_statement(s);
        if (st)
            ast_add_child(node, st);
        else
            break;
    }
    accept_symbol(s, '}');
    return node;
}

static AST *parse_call(PState *s, Lex *ident) {
    AST *node = ast_new(AST_CALL);
    node->name = dupstr(ident->token.text);
    accept_symbol(s, '(');
    while (peek(s) && !(peek(s)->type == L_SYMBOL && peek(s)->token.text &&
                        peek(s)->token.text[0] == ')')) {
        AST *arg = parse_expression(s);
        if (arg)
            ast_add_child(node, arg);
        if (accept_symbol(s, ','))
            continue;
        else
            break;
    }
    accept_symbol(s, ')');
    return node;
}

static AST *parse_expression(PState *s) {
    Lex *l = peek(s);
    if (!l)
        return NULL;
    AST *left = NULL;
    if (l->type == L_STRING) {
        advance(s);
        AST *lit = ast_new(AST_STRING);
        lit->str = dupstr(l->token.text);
        left = lit;
    } else if (l->type == L_IDENT) {
        Lex *id = advance(s);
        if (peek(s) && peek(s)->type == L_SYMBOL && peek(s)->token.text &&
            peek(s)->token.text[0] == '(') {
            left = parse_call(s, id);
        } else {
            AST *v = ast_new(AST_IDENT);
            v->name = dupstr(id->token.text);
            left = v;
        }
    } else {
        return NULL;
    }

    while (peek(s) && peek(s)->type == L_SYMBOL && peek(s)->token.text &&
           peek(s)->token.text[0] == '+') {
        accept_symbol(s, '+');
        AST *right = parse_expression(s);
        AST *cat = ast_new(AST_CONCAT);
        ast_add_child(cat, left);
        if (right)
            ast_add_child(cat, right);
        left = cat;
    }
    return left;
}

static AST *parse_statement(PState *s) {
    Lex *l = peek(s);
    if (!l)
        return NULL;
    if (l->type == L_KEYWORD) {
        if (strcmp(l->token.text, "var") == 0) {
            advance(s);
            return parse_var_decl(s);
        }
        if (strcmp(l->token.text, "func") == 0) {
            advance(s);
            return parse_func_def(s);
        }
        if (strcmp(l->token.text, "print") == 0) {
            advance(s);
            AST *call = parse_call(
                s, &(Lex){.type = L_IDENT,
                          .token = {.text = strdup("print"), .len = 5}});
            return call;
        }
    }
    if (l->type == L_IDENT) {
        Lex id = *advance(s);
        if (peek(s) && peek(s)->type == L_SYMBOL && peek(s)->token.text &&
            peek(s)->token.text[0] == '(') {
            return parse_call(s, &id);
        }
    }
    return NULL;
}

void free_ast(AST *a) {
    if (!a)
        return;
    free(a->name);
    free(a->str);
    for (size_t i = 0; i < a->param_count; ++i)
        free(a->params[i]);
    free(a->params);
    for (size_t i = 0; i < a->child_count; ++i)
        free_ast(a->children[i]);
    free(a->children);
    free(a);
}
