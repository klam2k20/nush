#ifndef AST_H
#define ASH_H

#include <stdlib.h>
#include "svec.h"
#include "hashmap.h"

typedef struct ast{
    char* op;
    struct ast* left;
    struct ast* right;
    svec* args;
}ast;

int check_syscall(int rv);
ast* make_args(svec* args);
ast* make_op(char* op, ast* left, ast* right);
void free_ast(ast* ast);
int evaluate_ast(ast* ast, hashmap* hh);
void print_ast(ast* ast);
#endif

