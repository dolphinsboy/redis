#include <stdint.h>

#ifndef __DICT_H
#define __DICT_H

#define DICT_OK 0
#define DICT_ERR 1

typedef struct dictEntry{
    //主键值
    void *key;
    //主键值
    union {
        void *val;
        uint64_t u64;
        int64_t s64;
        double d;
    }v;

    //使用链表解决冲突问题
    struct dictEntry *next;
}dictEntry;

typedef struct dictType{
    //Hash函数指针函数
    unsigned int (*hashFunction)(const void*key);
    //主键以及值复制
    void*(*keyDup)(void *privdata, const void*key);
    void*(*valDup)(void *privdata, const void*val);
    //主键比较
    int (*keyCompare)(void *privdata, const void*key1, const void*key2);
    //主键的析构函数
    void (*keyDestructor)(void *privdata, void *key);
    //主键值的析构函数
    void (*valDestructor)(void *privdata, void *val);
}dictType;

typedef struct dictht{
    //单个hash表
    dictEntry **table;
    //可容纳key的个数
    unsigned long size;
    //
    unsigned long sizemask;
    //已经存储key的个数
    unsigned long used;
}dictht;

typedef struct dict{
    dictType *type;
    //2个hash表
    dictht ht[2];
    //
    void *privdata;
    //当前是否在进行rehash标识
    long rehashidx;
    //iter计数器
    int iterators;
}dict;

//字典迭代器
typedef struct dictIterator{
    //当前字典
    dict *d;
    //迭代器流水
    long index;
    //
    dictEntry *entry, *nextEntry;
    //
    long long figerprint;
}dictIterator;

//
typedef void (dictScanFunction)(void *privdata, const dictEntry*de);

//初始化dictht中table数组个数
#define DICT_HT_INITIAL_SIZE  4
#define dictCompareKeys(d, key1, key2) \
    ((d)->type->keyCompare) ? \
    ((d)->type->keyCompare((d)->privdata, (key1), (key2))) : (key1 == key2)

#define dictIsRehashing(d) ((d)->rehashidx != -1)
#define dictHashKey(d, key) ((d)->type->hashFunction(key))
#define dictSetKey(d, entry, _key_) do { \
    if ((d)->type->keyDup) \
        entry->key = (d)->type->keyDup(d->privdata, (_key_)); \
    else \
        entry->key = _key_; \
}while(0);

#define dictSetVal(d, entry, _val_) do { \
    if ((d)->type->valDup) \
        entry->v.val = (d)->type->valDup(d->privdata, (_val_)); \
    else \
        entry->v.val = _val_; \
}while(0);


//--------API--------------
dict *dictCreate(dictType * type, void *privDataPtr);
int dictExpand(dict *d, unsigned long size);
int dictAdd(dict *d, void *key, void *val);
dictEntry *dictAddRaw(dict *d, void *key);

#endif
