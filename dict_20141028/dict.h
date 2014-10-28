#include <stdint.h>

#ifndef __DICT_H
#define __DICT_H

#define DICT_OK 0
#define DICT_ERROR 1
typedef struct dictType{
    unsigned int (*hashFunction)(const void *key);
}dictType;

typedef struct dictEntry{
    //键
    void *key;

    //值
    union {
        void *val;
        uint64_t u64;
        int64_t s64;
    }v;

    //链往后继节点
    struct dictEntry *next;
}dictEntry;

typedef struct dictht{
    //哈希桶节点指针数组,桶,bucket
    dictEntry **table;

    //指针数组的大小
    unsigned long size;

    //指针数组的长度掩码,用于计算索引值
    unsigned long sizemask;

    //哈希表现有的节点数量
    unsigned long used;
}dictht;

typedef struct dict{
    //特定类型的处理函数
    dictType *type;

    //类型处理函数的私有函数
    void *privdata;

    //哈希表,2个
    dictht ht[2];

    //记录rehash进度的标志,值为-1表示rehash未进行
    int rehashidx;

    //当前正在运行的安全迭代数量
    int iterators;
} dict;

dict * dictCreate(dictType* type,void *privDataPtr);

#endif
