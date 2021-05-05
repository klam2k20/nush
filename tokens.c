#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "svec.h"

/*
 * In this assignment I have decided to ignore any erronenous  user input
 * such as >>>. This will hopefully be handled in parse.c in the 
 * challenge if time permits
 */

int 
issop(int o)
{
    return ((o == '|') || (o == '&') || (o == ';') || (o == '>') || (o == '<') || (o == '=') || (o == '$'));
}

char* read_helper(const char* line, int ii, int ss)
{
    char* tt = malloc(ss + 1);
    memcpy(tt, line + ii, ss);
    tt[ss] = 0;
    return tt;
}

/*
 * The following function used Professor
 * Tuck's read_digit as a reference.
*/
char*
read_sop(const char* line, int ii)
{
    int ll = 0;
    while(issop(line[ii + ll]))
    {
        ++ll;
    }

    char* sop = read_helper(line, ii, ll);
    return sop;
}

/*
 * The following function used Professor
 * Tuck's read_digit as a reference.
*/
char*
read_sin(const char* line, int ii)
{
    int ll = 0;
    while(!issop(line[ii + ll]) && !isspace(line[ii + ll])) 
    {
        ++ll;
    }

    char* sin =  read_helper(line, ii, ll);
    return sin;
}

/*
 * The following function used Professor
 * Tuck's tokenize as a reference.
*/
svec*
tokenize(char* line)
{
    svec* tt = make_svec();
    int ii = 0;
    while(ii < strlen(line))
    {
        if(isspace(line[ii]))
        {
            ++ii;
            continue;
        }
        else if(issop(line[ii]))
        {
            char* sop = read_sop(line, ii);
            svec_push_back(tt, sop);        
            ii = ii + strlen(sop);
            free(sop);
            continue;
        }
        else 
        {
            char* sin = read_sin(line, ii);
            svec_push_back(tt, sin);
            ii =  ii + strlen(sin);
            free(sin);
        }
    }
    return tt;
}

//int
//main(int ac, char** av)
//{
//    char buffer[100];
//    while (1) {
//        printf("tokens$ ");
//        fflush(stdout);
//        char* line = fgets(buffer, 100, stdin);
//        if (line == NULL) {
//            printf("\n");
//            exit(0);
//        }
//        svec* tokens = tokenize(line);
//        for(int ii = tokens->size-1; ii >= 0; --ii)
//        {
//            printf("%s\n", tokens->data[ii]);
//        }    
//        free_svec(tokens);
//    }
//    return 0;
//}





