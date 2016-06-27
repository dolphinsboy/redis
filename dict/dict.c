#include <stdlib.h>
#include <limits.h>

#include "dict.h"

static unsigned long _dictNextPower(unsigned long size);
static void _dictReset(dictht *ht);
static int _dictInit(dict *d, dictType *type, void *privDataPtr);
static int _dictExpandIfNeeded(dict *d);

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

static int _dictExpandIfNeeded(dict *d){
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

int dictAdd(dict *d, void *key, void *val){
    dictEntry *entry = dictAddRaw(d, key);

    if(entry == NULL) return DICT_ERR;

    dictSetVal(d, entry, val);
    return DICT_OK;
}

static void _dictRehashStep(dict *d){
    //if(d->iterators == 0) dictRehash(d,1);
    if(d->iterators == 0) return;
}

//如果对应的key存在返回-1
static int _dictKeyIndex(dict *d, void *key){
    unsigned int h, idx, table;
    dictEntry *he;

    if(_dictExpandIfNeeded(d) == DICT_ERR)
        return -1;

    h = dictHashKey(d, key);

    for(table = 0; table <= 1; table++){
        idx = h & d->ht[table].sizemask;
        he = d->ht[table].table[idx];
        while(he){
            if(dictCompareKeys(d, key, he->key))
                return -1;
            he = he->next;
        }
    }

    return idx;
}

dictEntry *dictAddRaw(dict *d, void *key){
    int index;
    dictht *ht;
    dictEntry *entry;

    if(dictIsRehashing(d)) _dictRehashStep(d);
    //判断是否存在
    if((index = _dictKeyIndex(d, key)) == -1)
        return NULL;

    ht = dictIsRehashing(d) ? &d->ht[1] : &d->ht[0];
    entry = malloc(sizeof(*entry));
    entry->next = ht->table[index];
    ht->used++;
    ht->table[index] = entry;

    //设置键
    dictSetKey(d, entry, key);
    return entry;
}
