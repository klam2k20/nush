/* This file is lecture notes from CS 3650, Fall 2018 */
/* Author: Nat Tuck */

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "svec.h"

svec*
make_svec()
{
    svec* sv = malloc(sizeof(svec));
    sv->data = malloc(2 * sizeof(char*));
    sv->size = 0;
    sv->capacity = 2;
    return sv;
}

void
free_svec(svec* sv)
{
    for(int ii=0; ii<sv->size; ++ii)
    {
        free(sv->data[ii]);
    }
    free(sv->data);
    free(sv);
}

char*
svec_get(svec* sv, int ii)
{
    assert(ii >= 0 && ii < sv->size);
    return sv->data[ii];
}

void
svec_put(svec* sv, int ii, char* item)
{
    assert(ii >= 0 && ii < sv->size);
    sv->data[ii] = strdup(item);
}

void
svec_push_back(svec* sv, char* item)
{
    int ii = sv->size;
    int cc = sv->capacity;
    if(ii == cc) 
    {
        sv->capacity = sv->capacity * 2;
        sv->data = realloc(sv->data, sv->capacity * sizeof(char*));
    }
    sv->size = ii + 1;
    svec_put(sv, ii, item);
}

void
svec_swap(svec* sv, int ii, int jj)
{
    char* temp = svec_get(sv,ii);
    char* temp2 = svec_get(sv, jj);
    svec_put(sv, ii, temp2);
    svec_put(sv, jj, temp);
    free(temp);
    free(temp2);
}

svec*
svec_copy(svec* sv)
{
    svec* ss = make_svec();
    for(int ii=0; ii < sv->size; ++ii)
    {
        svec_push_back(ss, sv->data[ii]);
    }
    return ss;
}
