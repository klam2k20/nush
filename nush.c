#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "svec.h"
#include "tokens.h"
#include "ast.h"
#include "hashmap.h"

/*
 * Checks whether the given string is one of 
 * the following operations: &, |, &&, ||, ;, >
 * or <
*/
int 
isOp(char* cc)
{
    return(strcmp(cc, "<") == 0 ||  strcmp(cc, ">") == 0 || strcmp(cc, "|") == 0 || strcmp(cc, "||") == 0 ||
        strcmp(cc, "&") == 0 || strcmp(cc, "&&") == 0 || strcmp(cc, ";") == 0 || strcmp(cc, "=") == 0 || 
        strcmp(cc, "$") == 0) ; 
}

/*
 * Creates an ast tree from
 * the given array.
*/
ast*
make_ast(svec* ss, hashmap* hh)
{
    ast* root = make_args(NULL);
    int opIndex;
    svec* left = make_svec();
    svec* right = make_svec();
    for(int ii = 0; ii < ss->size; ++ii)
    {
        // If there are no operations make the root the 
        // left array
        if(isOp(ss->data[ii]) || (ii == ss->size - 1))
        {
            if(!isOp(ss->data[ii]))
            {   
                svec_push_back(left, ss->data[ii]);
                svec* leftCopy = svec_copy(left);
                root->args = leftCopy;
                break;
            }

            opIndex = ii;
            
    
            // Handle alias variables
            if (strcmp(ss->data[ii], "$") == 0)
            {
                if(hashmap_has(hh, ss->data[ii + 1]))
                {
                    char* vv = hashmap_get(hh, ss->data[ii + 1]);
                    svec_push_back(left, vv);
                    ++ii;
                    if(ii == ss->size -1)
                    {
                        root->args = svec_copy(left);
                        break;
                    }
                    continue;
                }
            }

            for(int xx = ii + 1; xx < ss->size; ++xx)
            {
                if(!isOp(ss->data[xx]))
                {
                    svec_push_back(right, ss->data[xx]);
                }
                else 
                {
                    break;
                }
            }
            ii += right->size;
            
            if(strcmp(root->op, "0") == 0)
            {
                svec* leftCopy = svec_copy(left);
                root->args = leftCopy;
                free_svec(left);
                left = make_svec();
            }
            
            // If an operation was found create a "node" connecting
            // the old root and right arguments with the operation found
            svec* rightCopy = svec_copy(right);
            root = make_op(ss->data[opIndex], root, make_args(rightCopy));
            free_svec(right);
            right = make_svec();
        }
        
        else 
        {
            // Push any non-operation tokens into the 
            // left array
            svec_push_back(left, ss->data[ii]);
        }
    }

    free_svec(right);
    free_svec(left);
    return root;
}

int
main(int argc, char* argv[])
{
    hashmap* hh = make_hashmap(); 
    FILE* file;
    if(argc > 1)
    {
        file = fopen(argv[1], "r");
    }
    
    char cmd[256];
    char* line;
    while(1)
    {
        if (argc == 1) {
            printf("nush$ ");
            fflush(stdout);
            line = fgets(cmd, 256, stdin);
        }
        else {
            line = fgets(cmd, 256, file);
        }
     
        if(line == NULL)
        {
            // File was given 
            if (argc > 1)
            {
                int rv = fclose(file);
                check_syscall(rv);
                wait(0);
                exit(0);
            }
            // No file given -> interactive mode
            if(argc == 1)
            {
                exit(0);
            }
        }

        svec* tokens = tokenize(cmd);
        ast* aa = make_ast(tokens, hh);
        
        if(tokens->size > 0)
        {
           // print_ast(aa);
            evaluate_ast(aa, hh);
            free_ast(aa);
            free_svec(tokens);
        }
    }
    
    free(line);
    free_hashmap(hh);
    return 0;
}
