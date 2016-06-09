#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "sds.h"

sds sdsnewlen(const void*init, size_t initlen){
    struct sdsadr *sh;

    //init为空,也生成对应的sdsadr对象
    if(init)
        sh = malloc(sizeof(*sh)+initlen+1);
    else
        sh = calloc(sizeof(*sh)+initlen+1, 1);

    if(sh == NULL) return NULL;

    sh->len = initlen;
    sh->free = 0;
    if(initlen&&init)
        memcpy(sh->buf, init, initlen);
    sh->buf[initlen]='\0';

    return (char*)sh->buf;
}

sds sdsempty(void){
    return sdsnewlen("",0);
}

sds sdsnew(const char * init){
    size_t initlen = (init == NULL) ? 0 : strlen(init);
    return sdsnewlen(init, initlen);
}

sds sdsdup(const sds s){
    return sdsnewlen(s, strlen(s));
}

void sdsfree(const sds s){
    if(s == NULL)return;

    free((s-sizeof(struct sdsadr)));
}

void sdsupdatelen(sds s){
    struct sdsadr * sh = (struct sdsadr *)(s - sizeof(struct sdsadr));
    size_t reallen = strlen(s);

    sh->len = reallen;
    sh->free += reallen;
}

void sdsclear(sds s){
    struct sdsadr *sh = (struct sdsadr *)(s - sizeof(struct sdsadr));
    size_t reallen = 0;
    sh->free += sh->len;
    sh->len = 0;

    sh->buf[0]='\0';
}

sds sdsMakeRoomFor(sds s, size_t addlen){
    struct sdsadr *sh;
    size_t len, newlen;
    size_t free = sdsavail(s);

    sh = (struct sdsadr*)(s-sizeof(struct sdsadr));

    len = sh->len;

    if(addlen < free) return s;
    
    newlen = len + addlen;
    //预分配
    if(newlen < SDS_MAX_PREALLOC)
        newlen *= 2;
    else
        newlen += SDS_MAX_PREALLOC;

    struct sdsadr *newsh;
    //扩展内存
    newsh = realloc(sh, newlen);

    if(newsh == NULL)return NULL;

    newsh->free = (newlen-len);

    return newsh->buf;
}

sds sdsRemoveFreeSpace(sds s){
    struct sdsadr *sh;
    size_t len;
    sh = (struct sdsadr*)(s - sizeof(struct sdsadr));

    len = sh->len;
    sh = realloc(sh, sizeof(struct sdsadr)+len+1);
    sh->free = 0;

    return sh->buf;
}

size_t sdsAllocSize(sds s){
    struct sdsadr *sh;
    sh = (struct sdsadr*)(s - sizeof(struct sdsadr));

    return sizeof(*sh)+sh->len+sh->free+1;
}

void sdsIncrLen(sds s, int incr){
    //添加sdsIncr的函数操作,其在现有内存空间中进行操作,不涉及内存的分配
    //故有别与sdsMakeRoomFor
    struct sdsadr *sh;
    sh = (struct sdsadr*)(s - sizeof(struct sdsadr));

    if(incr>=0){
        //增长的空间小于free,可以无需添加即可
        assert(sh->free >= (unsigned int)incr);
    }else{
        //现有的len如果大于incr的长度,才可以操作
        assert(sh->len >= (unsigned int)(-incr));
    }

    sh->len += incr;
    sh->free -= incr;

    s[sh->len] = '\0';
}

sds sdsgrowzero(sds s, size_t len){
    struct sdsadr *sh = (void*)(s-sizeof(struct sdsadr));
    size_t curlen = sh->len;
    size_t totallen;

    if(len <= curlen) return s;
    s = sdsMakeRoomFor(s, len-curlen);
    if(s == NULL) return NULL;

    sh = (void*)(s-sizeof(struct sdsadr));
    memset(sh->buf, 0 ,sh->len);

    totallen = sh->len + sh->free;
    sh->len = len;
    sh->free = totallen - sh->len;

    return s;
}

sds sdscatlen(sds s, const void *t, size_t len){
    struct sdsadr *sh;
    size_t currlen = sdslen(s);

    s = sdsMakeRoomFor(s,len);
    if(s == NULL) return NULL;

    sh = (void*)(s-sizeof(*sh));
    memcpy(s+currlen, t, len);
    sh->len = currlen + len;
    sh->free -= len;

    sh->buf[sh->len]='\0';

    return s;
}

sds sdscat(sds s, const sds t){
    return sdscatlen(s, t, sdslen(t));
}
