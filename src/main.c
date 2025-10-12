#include "lexer/lexer.h"
#include "parser/parser.h"
#include "runtime/runtime.h"
#include "tokenizer/tokenizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *read_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f)
        return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(sz + 1);
    fread(buf, 1, sz, f);
    buf[sz] = '\0';
    fclose(f);
    return buf;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s file.jail\n", argv[0]);
        return 2;
    }
    char *src = read_file(argv[1]);
    if (!src) {
        fprintf(stderr, "could not read %s\n", argv[1]);
        return 2;
    }

    TokenList tl = tokenize(src);
    LexList ll = lex(tl);
    AST *ast = parse(ll);
    run_ast(ast);

    free(src);
    free_tokens(&tl);
    free_lex(&ll);
    free_ast(ast);
    return 0;
}
