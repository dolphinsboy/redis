#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <assert.h>

#include "dict.h"

//主要用于是否进行dict的Rehash操作
static int dict_can_resize = 1; 
static unsigned int dict_force_resize_ratio = 5; 

/*--------------私有方法--------------------*/
//用于获取刚大于size的2倍数的expand大小
static unsigned long _dictNextPower(unsigned long size);
//dictInit结构体初始化
static int _dictInit(dict *d, dictType *type, void *privDataPtr);
//判断dict的ht是否需要扩容
static int _dictExpandIfNeeded(dict *d);
//获取key对应的在ht[table]中的位置
static int _dictKeyIndex(dict *d, void *key);

/*------------Hash函数相关--------------------*/
//没有发现使用的地方
/* Thomas Wang's 32 bit Mix Function */
unsigned int dictIntHashFunction(unsigned int key) 
{
    key += ~(key << 15); 
    key ^=  (key >> 10); 
    key +=  (key << 3);
    key ^=  (key >> 6);
    key += ~(key << 11); 
    key ^=  (key >> 16); 
    return key; 
}

static uint32_t dict_hash_function_seed = 5381;

//设置hash函数的seed,在redis启动的时候进行调用
void dictSetHashFunctionSeed(uint32_t seed){
	dict_hash_function_seed = seed;
}

uint32_t dictGetHashFunctionSeed(){
    return dict_hash_function_seed;
}
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

unsigned int dictGenCaseHashFunction(const unsigned char *buf, int len){
    unsigned int hash = (unsigned int)dictGetHashFunctionSeed();

    while(len--)
        hash = ((hash << 5) + hash) + (tolower(*buf++));
    
    return hash;
}

/*----------------API-----------------------*/
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

//将dict重新resize到used大小(具体值根据2的倍数来判断)
int dictResize(dict *d){
   int minimal;

   if(!dict_can_resize ||IsRehashing(d)) return DICT_ERR;

   minimal = d->ht[0].used;

   if(minimal < DICT_HT_INITIAL_SIZE)
       minimal = DICT_HT_INITIAL_SIZE;

   return dictExpand(d, minimal);
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


static int _dictExpandIfNeeded(dict *d)
{
    /* Incremental rehashing already in progress. Return. */
    if (dictIsRehashing(d)) return DICT_OK;

    /* If the hash table is empty expand it to the initial size. */
    if (d->ht[0].size == 0) return dictExpand(d, DICT_HT_INITIAL_SIZE);

    /* If we reached the 1:1 ratio, and we are allowed to resize the hash
     * table (global setting) or we should avoid it but the ratio between
     * elements/buckets is over the "safe" threshold, we resize doubling
     * the number of buckets. */
    if (d->ht[0].used >= d->ht[0].size &&
        (dict_can_resize ||
         d->ht[0].used/d->ht[0].size > dict_force_resize_ratio))
    {
        return dictExpand(d, d->ht[0].used*2);
    }
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
    if(d->iterators == 0) dictRehash(d,1);
}

//设置dictRehash,作用待明天研究20160705
//否则会影响_dictKeyIndex函数
//对于dict.table[1] 的访问
int dictRehash(dict *d, int n) {
    int empty_visits = n*10; /* Max number of empty buckets to visit. */
    if (!dictIsRehashing(d)) return 0;

    while(n-- && d->ht[0].used != 0) {
        dictEntry *de, *nextde;

        /* Note that rehashidx can't overflow as we are sure there are more
         * elements because ht[0].used != 0 */
        assert(d->ht[0].size > (unsigned long)d->rehashidx);
        while(d->ht[0].table[d->rehashidx] == NULL) {
            d->rehashidx++;
            if (--empty_visits == 0) return 1;
        }
        de = d->ht[0].table[d->rehashidx];
        /* Move all the keys in this bucket from the old to the new hash HT */
        while(de) {
            unsigned int h;

            nextde = de->next;
            /* Get the index in the new hash table */
            h = dictHashKey(d, de->key) & d->ht[1].sizemask;
            de->next = d->ht[1].table[h];
            d->ht[1].table[h] = de;
            d->ht[0].used--;
            d->ht[1].used++;
            de = nextde;
        }
        d->ht[0].table[d->rehashidx] = NULL;
        d->rehashidx++;
    }

    /* Check if we already rehashed the whole table... */
    if (d->ht[0].used == 0) {
        free(d->ht[0].table);
        d->ht[0] = d->ht[1];
        _dictReset(&d->ht[1]);
        d->rehashidx = -1;
        return 0;
    }

    /* More to rehash... */
    return 1;
}

//如果对应的key存在返回-1
static int _dictKeyIndex(dict *d, void *key){
    unsigned int h, idx, table;
    dictEntry *he;

    if(_dictExpandIfNeeded(d) == DICT_ERR)
        return -1;

    h = dictHashKey(d, key);

    for(table = 0; table < 1; table++){
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

dictEntry *dictFind(dict *d, void *key){
    unsigned int h, idx, table;
    dictEntry *he;

    if(d->ht[0].size == 0) return NULL;

    h = dictHashKey(d, key);

    for(table = 0; table <= 1; table++){
        idx = h & d->ht[table].sizemask;
        he = d->ht[table].table[idx];

        while(he){
            if(dictCompareKeys(d, key, he->key))
                return he;
            he = he->next;
        }
    }

    return NULL;
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
