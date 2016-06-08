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

static inline sdslen(const sds s){
    struct sdsadr *sh = (void*)(s-sizeof(struct sdsadr));
    return sh->len;
}

static inline sdsavail(const sds s){
    struct sdsadr *sh = (void*)(s-sizeof(struct sdsadr));
    return sh->free;
}

sds sdsnewlen(const void *init, size_t initlen);
sds sdsnew(const char *init);

#endif
