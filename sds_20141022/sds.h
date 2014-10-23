#ifndef __SDS_H
#define __SDS_H

#define SDS_MAX_PREALLOC (1024*1024)

typedef char *sds;

struct sdshdr {
    int len;
    int free;
    char buf[];
}sdshdr;

static inline size_t sdslen(const sds s) {
    struct sdshdr *sh = (void*)(s-(sizeof(struct sdshdr)));
    return sh->len;
}

static inline size_t sdsavail(const sds s) {
    struct sdshdr *sh = (void*)(s-(sizeof(struct sdshdr)));
    printf("%s",sh->buf);
    return sh->free;
}

sds sdsMakeRoomFor(sds s,size_t addlen);
#endif
