#include <stdint.h>

#ifndef __DICT_H
#define __DICT_H

#define DICT_OK 0
#define DICT_ERR 1

typedef struct dictEntry{
    //键
    void *key;

    //值
    union{
        void *val;
        uint64_t u64;
        int64_t s64;
        double d;
    }v;

    //指向下个哈希表节点的指针形成链表，冲突解决方案
    struct dictEntry *next;
} dictEntry;

typedef struct dictType{
    //计算哈希值的函数
    unsigned int (*hashFunction)(const void*key);
    //复制键的函数
    void *(*keyDup)(void *privdata, const void *key);
    //复制值的函数
    void *(*valDup)(void *privdata, const void *obj);
    //对比键的函数
    int (*keyCompare)(void *privdata, const void*key);
    //销毁键的函数
    void (*keyDestructor)(void *privdata, void *key);
    //销毁值的函数
    void (*valDestructor)(void *privdata, void *obj);
}dictType;

typedef struct dictht{
    //哈希表数组
    dictEntry **table;
    //哈希表大小
    unsigned long size;
    //哈希表大小掩码,等于size-1
    unsigned long sizemask;
    //哈希表已有节点的数目
    unsigned long used;
}dictht;

typedef struct dict{
    //类型特定函数
    dictType *type;
    //私有类型
    void *privdata;
    //哈希表
    dictht ht[2];

    int rehashidx;
    int iterators;
}dict;

typedef struct dictIterator{
    dict *d;
    int table, index, safe;
    dictEntry *entry, *nextEntry;
    long long fingerprint;
}dictIterator;

#define DICT_HT_INITIAL_SIZE 4

//Macros
#define dictFreeVal(d, entry) \
    if((d)->type->valDestructor) \
        (d)->type->valDestructor((d)->privdata, (entry)->v.val)

#define dictSetVal(d, entry, _val_) do { \
    if ((d)->type->valDup) \
        entry->v.val = (d)->type->valDup((d)->privdata,_val_); \
    else \
        entry->v.val = (_val_); \
}while(0)

#define dictSetSignedIntergerVal(entry, _val_) \
    do { entry->v.s64 = _val_; } while(0)

#define dictSetUnsginedIntegerVal(entry, _val_) \
    do { entry->v.u64 = _val_; } while(0)

#define dictSetDoubleVal(entry, _val_) \
    do { entry->v.d = _val_; } while(0)

#define dictFreeKey(d, entry) \
    if ((d)->type->keyDestructor) \
        (d)->type->keyDestructor((d)->privdata, (entry)->key); 

#define dictSetKey(d, entry, _key_) do{ \
    if ((d)->type->keyDup) \
        entry->key = (d)->type->keyDup((d)->privdata, _key_); \
    else \
        entry->key = (_key_); \
}while(0)

#define dictCompareKeys(d, key1, key2) \
    (((d)->type->keyCompare) ? \
     (d)->type->keyCompare((d)->privdata, key1, key2) : \
     (key1) == (key2))

#define dictHashKey(d, key) (d)->type->hashFunction(key)
#define dictGetKey(he) ((he)->key)
#define dictGetVal(he) ((he)->v.val)
#define dictGetSignedIntegerVal(he) ((he)->v.s64)
#define dictGetUnsignedIntegerVal(he) ((he)->v.u64)
#define dictGetDoubleVal(he) ((he)->v.d)
#define dictSlots(d) ((d)->ht[0].size+(d)->ht[1].size)
#define dictSize(d) ((d)->ht[0].used+(d)->ht[1].used)
#define dictIsRehashing(d) ((d)->rehashidx != -1)

/* API */
dict *dictCreate(dictType *type, void *privDataPtr);
int dictExpand(dict *d, unsigned long size);
int dictAdd(dict *d, void *key, void *val);
dictEntry *dictAddRaw(dict *d, void *key);
#endif /*__DCIT_H*/
