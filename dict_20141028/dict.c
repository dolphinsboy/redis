#include <stdlib.h>
#include <string.h>

#include "dict.h"

static void _dictReset(dictht *ht)
{
    ht->table = NULL;
    ht->size = 0;
    ht->sizemask = 0;
    ht->used = 0;
}

int _dictInit(dict *d, dictType *type,void *privDataPtr)
{
    _dictReset(&d->ht[0]);
    _dictReset(&d->ht[1]);

    d->type = type;
    d->privdata = privDataPtr;

    d->rehashidx = -1;
    d->iterators = 0;

    return DICT_OK;
}

dict * dictCreate(dictType* type,void *privDataPtr)
{
    dict *d = malloc(sizeof(*d));
    _dictInit(d,type,privDataPtr);

    return d;
}
