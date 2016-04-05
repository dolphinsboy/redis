#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <sys/time.h>
#include <ctype.h>

#include "dict.h"

static void _dictReset(dictht *ht)
{
    ht->table = NULL;
    ht->size = 0;
    ht->sizemask = 0;
    ht->used = 0;
}

/* Create a new hash table */
dict *dictCreate(dictType *type, void *privDataPtr)
{
    dict *d = malloc(sizeof(*d));

    _dictInit(d, type, privDataPtr);
    return d;
}

/* Initialize the hash table */
int _dictInit(dict *d, dictType *type, void *privDataPtr)
{
    _dictReset(&d->ht[0]);
    _dictReset(&d->ht[1]);
    d->type = type;
    d->privdata = privDataPtr;
    d->rehashidx = -1;
    d->iterators = 0;

    return DICT_OK;
}

/* Our hash table capability is a power of two */
static unsigned long _dictNextPower(unsigned long size)
{
    unsigned long i = DICT_HT_INITIAL_SIZE;

    if (size >= LONG_MAX) return LONG_MAX;

    while(1){
        if (i >= size)
            return i;
        i *= 2;
    }
}

/*Expand or create the hash table*/
int dictExpand(dict *d, unsigned long size)
{
    dictht n; /*the new hash table*/
    unsigned long realsize = _dictNextPower(size);

    if (dictIsRehashing(d) || d->ht[0].used > size)
        return DICT_ERR;

    /* Rehasing to the same table size is not usefull */
    if (realsize == d->ht[0].size) return DICT_ERR;

    /* Allocate the new hash table and initialize all pointers to NULL */
    n.size = realsize;
    n.sizemask = realsize - 1;
    n.table = calloc(realsize, sizeof(dictEntry*));
    n.used = 0;

    if (d->ht[0].table == NULL){
        d->ht[0] = n;
        return DICT_OK;
    }

    /* Prepare a second hash table for incremental rehasing */
    d->ht[1] = n;
    d->rehashidx = 0;
    return DICT_OK;
}   

/* Add an element to the target hash table */
int dictAdd(dict *d, void *key, void *val)
{
    dictEntry *entry = dictAddRaw(d, key);

    if(!entry) return DICT_ERR;
    dictSetVal(d, entry, val);
    return DICT_OK;
}

dictEntry *dictAddRaw(dict *d, void *key)
{
    int index;
    dictEntry *entry;
    dictht *ht;

    if(dictIsRehashing(d)) _dictRehashStep(d);

    /* Get the index of the new element, or -1 if 
     * the element already exists. */
    if ((index = _dictKeyIndex(d, key)) == -1)
        return NULL;

    /* Allocate the memory and store the new entry */
    ht = dictIsRehashing(d) ? &d->ht[1] : &d->ht[0];
    entry = malloc(sizeof(*entry));
    entry->next = ht->table[index];
    ht->table[index] = entry;
    ht->used++;

    /*Set the hash entry files */
    dictSetKey(d, entry, key);
    return entry;
}
