#include "runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Var {
    char *name;
    char *value;
    struct Var *next;
} Var;
typedef struct Func {
    char *name;
    AST *node;
    struct Func *next;
} Func;

static Var *vars = NULL;
static Func *funcs = NULL;

static void set_var(const char *name, const char *val) {
    for (Var *v = vars; v; v = v->next)
        if (strcmp(v->name, name) == 0) {
            free(v->value);
            v->value = strdup(val);
            return;
        }
    Var *n = malloc(sizeof(Var));
    n->name = strdup(name);
    n->value = strdup(val);
    n->next = vars;
    vars = n;
}

static const char *get_var(const char *name) {
    for (Var *v = vars; v; v = v->next)
        if (strcmp(v->name, name) == 0)
            return v->value;
    return NULL;
}

static void register_func(const char *name, AST *node) {
    Func *f = malloc(sizeof(Func));
    f->name = strdup(name);
    f->node = node;
    f->next = funcs;
    funcs = f;
}

static Func *find_func(const char *name) {
    for (Func *f = funcs; f; f = f->next)
        if (strcmp(f->name, name) == 0)
            return f;
    return NULL;
}

static char *eval_expr(AST *expr);

static char *str_concat(const char *a, const char *b) {
    size_t la = a ? strlen(a) : 0;
    size_t lb = b ? strlen(b) : 0;
    char *r = malloc(la + lb + 1);
    if (a)
        memcpy(r, a, la);
    if (b)
        memcpy(r + la, b, lb);
    r[la + lb] = '\0';
    return r;
}

static char *eval_expr(AST *expr) {
    if (!expr)
        return strdup("");
    switch (expr->type) {
    case AST_STRING:
        return strdup(expr->str);
    case AST_IDENT: {
        const char *v = get_var(expr->name);
        return strdup(v ? v : "");
    }
    case AST_CONCAT: {
        char *left = eval_expr(expr->children[0]);
        char *right = eval_expr(expr->children[1]);
        char *res = str_concat(left, right);
        free(left);
        free(right);
        return res;
    }
    case AST_CALL: {
        if (strcmp(expr->name, "print") == 0) {
            if (expr->child_count > 0) {
                char *v = eval_expr(expr->children[0]);
                printf("%s\n", v);
                free(v);
            } else
                printf("\n");
            return strdup("");
        } else {
            Func *f = find_func(expr->name);
            if (!f)
                return strdup("");
            AST *fn = f->node;
            char **oldvals = malloc(sizeof(char *) * fn->param_count);
            for (size_t i = 0; i < fn->param_count; ++i) {
                const char *cur = get_var(fn->params[i]);
                oldvals[i] = cur ? strdup(cur) : NULL;
                char *argval = (i < expr->child_count)
                                   ? eval_expr(expr->children[i])
                                   : strdup("");
                set_var(fn->params[i], argval);
                free(argval);
            }
            for (size_t i = 0; i < fn->child_count; ++i) {
                AST *st = fn->children[i];
                if (st->type == AST_VAR_DECL) {
                    char *val = eval_expr(st->children[0]);
                    set_var(st->name, val);
                    free(val);
                } else if (st->type == AST_CALL) {
                    AST tmp = *st;
                    eval_expr(&tmp);
                }
            }
            for (size_t i = 0; i < fn->param_count; ++i) {
                if (oldvals[i])
                    set_var(fn->params[i], oldvals[i]);
                else {
                    set_var(fn->params[i], "");
                }
                free(oldvals[i]);
            }
            free(oldvals);
            return strdup("");
        }
    }
    default:
        return strdup("");
    }
}

int run_ast(AST *program) {
    if (!program || program->type != AST_PROGRAM)
        return 1;
    for (size_t i = 0; i < program->child_count; ++i) {
        AST *st = program->children[i];
        if (st->type == AST_FUNC_DEF) {
            register_func(st->name, st);
        }
    }
    for (size_t i = 0; i < program->child_count; ++i) {
        AST *st = program->children[i];
        if (st->type == AST_VAR_DECL) {
            char *val = eval_expr(st->children[0]);
            set_var(st->name, val);
            free(val);
        } else if (st->type == AST_CALL) {
            eval_expr(st);
        }
    }
    return 0;
}
