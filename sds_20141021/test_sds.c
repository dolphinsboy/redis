#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <sys/types.h>
//#include <stdarg.h>

typedef char *sds;

struct sdshdr{
    int len;
    int free;
    char buf[];
} sdshdr;

static inline size_t sdsavail(const sds s) {
    struct sdshdr *sh = (void*)(s-(sizeof(struct sdshdr)));
    return sh->free;
}

static inline size_t sdslen(const sds s)
{
    struct sdshdr *sh = (void*)(s-(sizeof(struct sdshdr)));
    printf("len_ptr=%d\n",&sh->len);
    printf("free_ptr=%d\n",&sh->free);
    printf("buf_ptr=%d\n",&sh->buf);
    return sh->len;
}

sds sdsMakeRoomFor(const sds s,size_t addlen)
{
    size_t free = sdsavail(s);
    size_t len;

    len = sdslen(s);
    if(free >= addlen)
        return s;
    printf("free=%d\n",free);
    return s;
}

int main(int argc, char** argv)
{
    sds pbuf = "Hexxxlloworld!\n";

    size_t free = sdsavail(pbuf);
    size_t len = sdslen(pbuf);

    printf("free=%d\n",free);
    printf("len=%d\n",len);

    //sds ps = sdsMakeRoomFor(pbuf,1000000000);
    //printf("ps=%s",ps);
    return 0;
}

