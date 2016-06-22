#include <stdlib.h>

#include "dict.h"

static void _dictReset(dictht *ht);
static int _dictInit(dict *d, dictType *type, void *privDataPtr);

dict *dictCreate(dictType *type, void *privDataPtr){
    dict *d = malloc(sizeof(*d));
    if(d == NULL) return NULL;

    _dictInit(d,type, privDataPtr);

    return d;
}

void _dictReset(dictht *ht){
    ht->table = NULL;
    ht->size = 0;
    ht->sizemask = 0;
    ht->used = 0;
}

int _dictInit(dict *d, dictType *type, void *privDataPtr){
    //->和[]优先级比&高
    //->和[]相同的优先级,从左到右结合
    //&d->ht[0]相当于&((d->ht)[0])
    //故dictReset参数为dictht*
    _dictReset(&d->ht[0]);
    _dictReset(&d->ht[1]);
    d->privdata = privDataPtr;
    d->type = type;
    d->rehashidx = -1;
    d->iterators = 0;

    return DICT_OK;
}
