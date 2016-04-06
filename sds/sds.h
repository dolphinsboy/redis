#ifndef __SDS_H
#define __SDS_H

#define SDS_MAX_MACLLOC (1024*1024)

#include <sys/types.h>

typedef char* sds;

struct sdshdr{
    int len;
    int free;
    char buf[];
};


static inline size_t sdslen(const sds s){
    //how structure to storage in memory?
    struct sdshdr *sh = (void*)(s-(sizeof(struct sdshdr)));
    return sh->len;
}

static inline size_t sdsavail(const sds s){
    struct sdshdr *sh = (void*)(s-(sizeof(struct sdshdr)));
    return sh->free;
}
sds sdsnew(const char* init);
#endif
