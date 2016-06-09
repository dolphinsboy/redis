#ifndef __SDS_H
#define __SDS_H

#define SDS_MAX_PREALLOC (1024*1024)
#include  <sys/types.h>
#include <stdarg.h>

typedef char *sds;

typedef struct sdsadr{
    unsigned int len;
    unsigned int free;
    char buf[];
} sdsadr;

static inline size_t sdslen(const sds s){
    struct sdsadr *sh = (void*)(s-sizeof(struct sdsadr));
    return sh->len;
}

static inline size_t sdsavail(const sds s){
    struct sdsadr *sh = (void*)(s-sizeof(struct sdsadr));
    return sh->free;
}

//创建指定长度的sds
sds sdsnewlen(const void *init, size_t initlen);
//根据字符串创建sds
sds sdsnew(const char *init);
//设置sds为空串
sds sdsempty();
//内联函数获取sds的len
size_t sdslen(const sds s);
//获取sds的free
size_t sdsavail(const sds s);
//复制sds,根据sds的字符串重新创建一个
sds sdsdup(const sds s);
//free sds
void sdsfree(const sds s);
//更新sds为其实际的长度
void sdsupdatelen(sds s);
//清空sds
void sdsclear(sds s);
//增加sds的可用空间
sds sdsMakeRoomFor(sds s, size_t addlen);
//清理free space
sds sdsRemoveFreeSpace(sds s);
//返回sds所有分配的所有空间大小
size_t sdsAllocSize(sds s);

#endif
