#include <stdlib.h>
#include <limits.h>

#include "dict.h"

static unsigned long _dictNextPower(unsigned long size);
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
//扩展或者创建dictht
int dictExpand(dict *d, unsigned long size){
    //声明新的hash表
    dictht n;
    unsigned long realsize = _dictNextPower(size);

    //size不能比实际还小
    if(dictIsRehashing(d) || d->ht[0].used > size)
        return DICT_ERR;

    //扩展的size和现有大小一样
    if(realsize == d->ht[0].size) return DICT_ERR;

    n.size = realsize;
    n.sizemask = realsize - 1;
    n.used = 0;
    //dictEntry*数组的大小为realsize
    n.table = calloc(1,sizeof(dictEntry*)*realsize);

    if(d->ht[0].table == NULL){
        d->ht[0] = n;
        return DICT_OK;
    }

    d->ht[1] = n;
    d->rehashidx = 0;

    return DICT_OK;
}

unsigned long _dictNextPower(unsigned long size){
    unsigned long i = DICT_HT_INITIAL_SIZE;
    if(size > LONG_MAX) return LONG_MAX;
    //指数增长
    while(1){
        if(i>=size)
            return i;
        i *= 2;
    }

}
