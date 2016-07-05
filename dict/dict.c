#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "dict.h"

static unsigned long _dictNextPower(unsigned long size);
static void _dictReset(dictht *ht);
static int _dictInit(dict *d, dictType *type, void *privDataPtr);
static int _dictExpandIfNeeded(dict *d);

static uint32_t dict_hash_function_seed = 5381;

/* MurmurHash2, by Austin Appleby
 * Note - This code makes a few assumptions about how your machine behaves -
 * 1. We can read a 4-byte value from any address without crashing
 * 2. sizeof(int) == 4
 *
 * And it has a few limitations -
 *
 * 1. It will not work incrementally.
 * 2. It will not produce the same results on little-endian and big-endian
 *    machines.
 */
unsigned int dictGenHashFunction(const void *key, int len) {
    /* 'm' and 'r' are mixing constants generated offline.
     They're not really 'magic', they just happen to work well.  */
    uint32_t seed = dict_hash_function_seed;
    const uint32_t m = 0x5bd1e995;
    const int r = 24;

    /* Initialize the hash to a 'random' value */
    uint32_t h = seed ^ len;

    /* Mix 4 bytes at a time into the hash */
    const unsigned char *data = (const unsigned char *)key;

    while(len >= 4) {
        uint32_t k = *(uint32_t*)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    /* Handle the last few bytes of the input array  */
    switch(len) {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0]; h *= m;
    };

    /* Do a few final mixes of the hash to ensure the last few
     * bytes are well-incorporated. */
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return (unsigned int)h;
}

unsigned int dictHashFunction(const void *key){
    return  dictGenHashFunction(key, strlen(key));
}   

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
int _dictKeyIndex(dict *d, void *key){
    unsigned int h, idx, table;
    dictEntry *he;

    if(_dictExpandIfNeeded(d) == DICT_ERR)
        return -1;

    h = dictHashKey(d, key);

    for(table = 0; table < 1; table++){
        idx = h & d->ht[table].sizemask;
        if (d->ht[table].table == NULL)
            return idx;

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
