
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <bsd/string.h>
#include <string.h>

#include "hashmap.h"

hashmap_pair*
make_pair(const char* kk, const char* vv, bool uu, bool tt)
{
    hashmap_pair* pp = malloc(sizeof(hashmap_pair));
    pp->val = strdup(vv);
    pp->used = uu;
    pp->tomb = tt;
    pp->key = strdup(kk);
    return pp;
}

void
free_pair(hashmap_pair* pp)
{
    free(pp->val);
    free(pp->key);
    free(pp);
}

void 
free_pairs(hashmap_pair** dd, int cc)
{
    for(int ii = 0; ii < cc; ++ii)
    {
        free_pair(dd[ii]);
    }
    free(dd);
}

/*
 * The following hash function is based off 
 * the hash function Professor Tuck
 * utilized in class when coding map.c
*/
int
hash(char* key, long cc)
{
    long hh = 0;
    for(long ii = 0; key[ii]; ++ii)
    {
        hh = hh * 13 + key[ii];
    }
    return hh & (cc - 1);
}

/*
 * The following function built on the map_grow
 * function Professor Tuck coded in class whenn
 * writing map.c
*/
void 
hashmap_grow(hashmap* hh)
{
    int cc = hh->capacity;
    hashmap_pair** data = hh->data;

    hh->capacity = 2 * cc;
    hh->size = 0;
    hh->data = calloc(hh->capacity, sizeof(hashmap_pair*));
    
    char* temp = "";
    for(int ii = 0; ii < hh->capacity; ++ii)
    {
        hh->data[ii] = make_pair(temp, temp, false, false);
    }
    for(int ii = 0; ii<cc; ++ii)
    {
        if(data[ii]->used)
        {
            hashmap_put(hh, data[ii]->key, data[ii]->val);
        }
    }
    free_pairs(data, cc);
}

hashmap*
make_hashmap_presize(int nn)
{
    hashmap* hh = malloc(sizeof(hashmap));
    hh->size = 0;
    hh->capacity = nn;
    hh->data = calloc(nn, sizeof(hashmap_pair*));
    
    char* temp = "";
    for(int ii = 0; ii < nn; ++ii)
    {
        hh->data[ii] = make_pair(temp, temp, false, false);
    }
    return hh;
}

hashmap*
make_hashmap()
{
    return make_hashmap_presize(4);
}

void
free_hashmap(hashmap* hh)
{
    free_pairs(hh->data, hh->capacity);
    free(hh);
}

int
hashmap_has(hashmap* hh, char* kk)
{
    return hashmap_get(hh, kk) != NULL;
}

char*
hashmap_get(hashmap* hh, char* kk)
{
    int xx = hash(kk, hh->capacity);
    while(hh->data[xx]->used || hh->data[xx]->tomb)
    {
        if(hh->data[xx]->used)
        {
            if(!(strcmp(hh->data[xx]->key, kk)))
            {
                return hh->data[xx]->val;
            }
        }
        xx = (xx + 1) & (hh->capacity - 1);
    }
    return NULL;
}

void
hashmap_put(hashmap* hh, char* kk, char* vv)
{
    int ss = hh->size + 1;
    int cc = hh->capacity;
    float load = (float)ss/cc;
    if(load > 0.5)
    {
        hashmap_grow(hh);
    }
    int xx = hash(kk, hh->capacity);
    while(hh->data[xx]->used)
    {
        if(!(strcmp(hh->data[xx]->key, kk)))
        {
            break;
        }
        xx= (xx+1) & (hh->capacity - 1);
    }
    hh->data[xx]->val = strdup(vv);
    hh->data[xx]->key = strdup(kk);
    hh->data[xx]->used = true;
    hh->data[xx]->tomb = false;
    hh->size++;
}

void
hashmap_del(hashmap* hh, char* kk)
{
    if(hashmap_has(hh, kk))
    {
        int xx = hash(kk, hh->capacity);
        while(hh->data[xx]->used || hh->data[xx]->tomb)
        {
            if(hh->data[xx]->used)
            {
                if(!(strcmp(hh->data[xx]->key, kk)))
                {
                    hh->data[xx]->used = false;
                    hh->data[xx]->tomb = true;
                    hh->size--;
                }
            }
            xx = (xx + 1) & (hh->capacity - 1);
        }   

    }
}

hashmap_pair
hashmap_get_pair(hashmap* hh, int ii)
{
    return *(hh->data[ii]);
}

void
hashmap_dump(hashmap* hh)
{
    printf("== hashmap dump ==\n");
    for(int ii = 0; ii < hh->capacity; ++ii)
    {   
        hashmap_pair* pp = hh->data[ii];
        printf(" %i , %s , %s , %i , %i \n", ii, pp->key, pp->val, pp->used, pp->tomb);
    }
}


