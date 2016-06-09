#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    if(newlen < SDS_MAX_PREALLOC)
        newlen *= 2;
    else
        newlen += SDS_MAX_PREALLOC;

    struct sdsadr *newsh;
    newsh = realloc(sh, newlen);

    if(newsh == NULL)return NULL;

    newsh->free = (newlen-len);

    return newsh->buf;
}
