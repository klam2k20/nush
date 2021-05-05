#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "svec.h"
#include "ast.h"

/* Uses Professor  Tuck's make_ast_value()
 * as a reference to create the structure 
 * of the function
 *
 * Creates a "leaf" in an ast tree with the 
 * array values given. A leaf represents a 
 * single command with no operations such as
 * echo one
*/
ast*
make_args(svec* ss) 
{
    ast* aa = malloc(sizeof(ast));
    aa->op = "0";
    aa->left = 0;
    aa->right = 0;
    aa->args = ss;
    return aa;
}

/* Uses Professor  Tuck's make_ast_op()
 * as a reference to create the structure 
 * of the function
 *
 * Creates a "node" in an ast tree with the 
 * given operation, and left and right trees.
 * A node represents a shell operation with its 
 * left and right inputs. An example would be
 * sort file.txt > file2.txt
*/
ast* 
make_op(char* op, ast* left, ast* right)
{
    ast* aa = malloc(sizeof(ast));
    aa->op = strdup(op);
    aa->left = left;
    aa->right = right;
    aa->args = 0;
    return aa;
}


/* Uses Professor  Tuck's free_ast
 * as a reference to create the structure 
 * of the function
 *
 * Frees all allocated memory in a given
 * ast tree
*/
void
free_ast(ast* aa)
{
    if(aa)
    {
        if(strcmp(aa->op,"0") == 0)
        {
            free_svec(aa->args);
        }
        if(aa->left)
        {
            free_ast(aa->left);
        }
        if(aa->right)
        {
            free_ast(aa->right);
        }
        if(strcmp(aa->op, "0") != 0)
        {
            free(aa->op);
        }
            
        free(aa);
    }
}

/*
 * Prints the ast tree. Used for 
 * testing purposes
*/
void 
print_ast(ast* aa)
{
    if(strcmp(aa->op, "0") == 0)
    {    
        for(int ii = 0; ii < aa->args->size; ++ii)
        {
            printf("\t ARGS: %s\n", aa->args->data[ii]);
        }
    }
    else 
    {  
        printf("OP: %s", aa->op);
        printf("LEFT: \n");
        print_ast(aa->left);
        printf("RIGHT: \n");
        print_ast(aa->right);
    }
}

/* Created by Professor Tuck 
 * in class on 2/23
 *
 * Checks if a system call fails or not
 */
int 
check_syscall(int rv)
{
    if (rv == -1) 
    {
        perror("System call error");
        exit(1);
    }
}

/*
 * Executes a single command. The function 
 * forks and passes the command to a child
 * subshell to handle
 * Ex: echo one
*/
int
evaluate_cmd(ast* aa)
{
    int cpid;

    if ((cpid = fork())) 
    {
        check_syscall(cpid);
        int status;
        waitpid(cpid, &status, 0);
        return (WEXITSTATUS(status));
    }

    else 
    {
        int size = aa->args->size;
        char* args[size + 1];
    
        for (int ii=0; ii<size; ++ii)
        {
            args[ii] = aa->args->data[ii];
        }
        args[size] = 0;
        execvp(args[0], args);
    }    
}

/* Handles any commands with '<'
*/
void
evaluate_redin(ast* aa, hashmap* hh)
{
    int cpid, rv;
    char* file = aa->right->args->data[0];
    if ((cpid = fork())) 
    {
        check_syscall(cpid);
        int status;
        waitpid(cpid, &status, 0);
    }

    else 
    {
        int fd = open(file, O_RDONLY, 0444);
        check_syscall(fd);
        rv = close(0);
        check_syscall(rv);
        rv = dup(fd);
        check_syscall(rv);
        rv = close(fd);
        check_syscall(rv);
        evaluate_ast(aa->left, hh);
        _exit(0);
    }
}

/* Uses Professor  Tuck's main in redir.c
 * as a reference to create the structure 
 * of the function
 *
 * Handles any commands with '>'
*/
void
evaluate_redout(ast* aa, hashmap* hh)
{
    int cpid, rv;
    char* file = aa->right->args->data[0];
    if ((cpid = fork())) {
        check_syscall(cpid);
        int status;
        waitpid(cpid, &status, 0);
    }

    else {
        int fd = open(file, O_CREAT | O_APPEND | O_WRONLY, 0644);
        check_syscall(fd);
        rv = close(1);
        check_syscall(rv);
        rv = dup(fd);
        check_syscall(rv);
        rv = close(fd);
        check_syscall(rv);
        evaluate_ast(aa->left, hh);
        _exit(0);
    }
}

/* Uses Professor  Tuck's main in pipe2.c
 * as a reference to create the structure 
 * of the function
 *
 * Handles any commands that include '|'
*/
void
evaluate_pipe(ast* aa, hashmap* hh)
{
    int cpid, rv;
    if ((cpid = fork())) {
        check_syscall(cpid);
        int status;
        waitpid(cpid, &status, 0);
    }

    else {
        int pipe_fds[2];
        rv = pipe(pipe_fds);
        check_syscall(rv);
        int p_read = pipe_fds[0];
        int p_write = pipe_fds[1];
    
        if((cpid = fork())) {
            check_syscall(cpid);
            rv = close(p_write);
            check_syscall(rv);
            rv = close(0);
            check_syscall(rv);
            rv = dup(p_read);
            check_syscall(rv);
            close(p_read);
            evaluate_ast(aa->right, hh);
            _exit(0);
        }
        
        else {
            rv = close(p_read);
            check_syscall(rv);
            rv = close(1);
            check_syscall(rv);
            rv  = dup(p_write);
            check_syscall(rv);
            close(p_write);
            evaluate_ast(aa->left, hh);
            _exit(0);
        }
    }
}


/* Handles any commands that include &
*/
void
evaluate_bg(ast* aa, hashmap* hh)

{
    int cpid, rv;
    if ((cpid = fork())) {
        check_syscall(cpid);
        if(aa->right->args->size > 0) 
        {
            evaluate_ast(aa->right, hh);
            _exit(0);
        }
    }

    else {
        evaluate_ast(aa->left, hh);
        _exit(0);
    }
}

/* Handles any commands that include ;
*/
void
evaluate_sc(ast* aa, hashmap* hh)
{
    if (aa->left)
    {
        evaluate_ast(aa->left, hh);
    }
    
    if (aa->right)
    {
        evaluate_ast(aa->right, hh);
    }
    
    _exit(0);
}

/* Handles any commands that include &&
*/
void
evaluate_and(ast* aa, hashmap* hh)
{
    int rv;
    rv = evaluate_ast(aa->left, hh);
    if (rv == 0) 
    {
        evaluate_ast(aa->right, hh);
    }
}

/* Handles any commands that include ||
*/
void
evaluate_or(ast* aa, hashmap* hh)
{
    int rv;
    rv = evaluate_ast(aa->left, hh);
    if (rv != 0) 
    {
        evaluate_ast(aa->right, hh);
    }
}

/*
 * Recursively evaluates an ast tree
 * by redirection to helper functions
 * above
*/
int 
evaluate_ast(ast* aa, hashmap* hh)
{
    int rv;
    if(strcmp(aa->op, "0") == 0)
    {
        if(strcmp(aa->args->data[0], "cd") == 0)
        {
            chdir(aa->args->data[1]);
        }
        
        else if(strcmp(aa->args->data[0], "exit") == 0)
        {
            free_hashmap(hh);
            free_ast(aa);
            exit(0);
        }
        
        else 
        {
            rv = evaluate_cmd(aa);
            return rv;
        }
    }
    
    else if(strcmp(aa->op, "<") == 0)
    {
        evaluate_redin(aa, hh);
    }      

    else if(strcmp(aa->op, ">") == 0)
    {
        evaluate_redout(aa, hh);
    }

    else if(strcmp(aa->op, "|") == 0)
    {
        evaluate_pipe(aa, hh);
    }

    else if(strcmp(aa->op, "&") == 0)
    {
       evaluate_bg(aa, hh);
    }      
    
    else if(strcmp(aa->op, "||") == 0)
    {
        evaluate_or(aa, hh);
    }      
    
    else if(strcmp(aa->op, "&&") == 0)
    {
        evaluate_and(aa, hh);
    }      
    
    else if(strcmp(aa->op, ";") == 0)
    {
        evaluate_sc(aa, hh);
    }     
   
    
    else if(strcmp(aa->op, "=") == 0)
    {
        hashmap_put(hh, aa->left->args->data[0], aa->right->args->data[0]);
    }     
    
    else 
    {
        exit(1);
    }
}


