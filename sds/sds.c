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
    return sdscatlen(s, t, strlen(t));
}

sds sdscpylen(sds s, const void *t, size_t len){
    struct sdsadr *sh = (void*)(s-sizeof(struct sdsadr));
    size_t totallen = sh->free+sh->len;

    if(totallen<len){
        s = sdsMakeRoomFor(s, len);
        sh = (void*)(s-sizeof(struct sdsadr));
        if(sh == NULL)
            return NULL;
        totallen = sh->free+sh->len;
    }

    memcpy(s, t, len);
    s[len]='\0';
    sh->len = len;
    sh->free = totallen - len;

    return s;
}

sds sdscpy(sds s , const void *t){
    return sdscpylen(s, t, strlen(t));
}

int sdsll2str(char *s, long long value){
    char *p, aux;
    unsigned long long v;
    size_t l;
    //按10取模将单个数字保存到buf中 
    v = (value < 0) ? -value : value;
    p = s;
    do{
        *p++ = '0' + v%10;
        v /= 10;
    }while(v);

    if(value<0)
        *p++ = '-';
    *p = '\0';

    l = p-s;

    //翻转字符串,需要排除结尾的\0
    p--;
    while(s<p){
        aux = *s;
        *s = *p;
        *p = aux;
        s++;
        p--;
    }

    return l;
}

int sdsull2str(char *s, unsigned long long value){
    char *p, aux;
    size_t l;
    p = s;
    //获取各位数字保存到s中
    do{
        *p++ = '0' + value%10;
        value /= 10;
    }while(value);

    l = p-s;
    *p = '\0';

    //翻转字符串
    p--;

    while(s<p){
        aux = *s;
        *s = *p;
        *p = aux;

        s++;
        p--;
    }

    return l;
}

#define SDS_LLSTR_SIZE 21
sds sdsfromlonglong(long long value){
    char buf[SDS_LLSTR_SIZE];
    int newlen;
    newlen = sdsll2str(buf, value);

    return sdsnewlen(buf, newlen);
}

sds sdsvprintf(sds s, const char *fmt, va_list ap){
   va_list cpy;
   char staticbuf[1024], *buf=staticbuf, *t;
   size_t buflen = strlen(fmt)*2;

   if(buflen >= sizeof(staticbuf)){
        buf = malloc(buflen);
        if(buf==NULL) return NULL;
   }else{
       buflen = sizeof(staticbuf);
   }

   while(1){
        buf[buflen-2] = '\0';
        va_copy(cpy, ap);
        vsnprintf(buf, buflen, fmt, cpy);
        va_end(cpy);

        if(buf[buflen-2]!='\0'){
            if(buf != staticbuf) free(buf);
            buflen *= 2;
            buf = malloc(buflen);
            if(buf == NULL) return NULL;
            continue;
        }
        break;
   }

   t = sdscat(s, buf);
   if(buf != staticbuf) free(buf);

   return t;
}

sds sdscatprintf(sds s, const char *fmt, ...){
    va_list ap;
    char *t;

    va_start(ap, fmt);
    t = sdsvprintf(s, fmt, ap);
    va_end(ap);

    return t;
}
