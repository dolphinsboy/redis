#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sds.h"

sds sdsMakeRoomFor(sds s,size_t addlen)
{
    struct sdshdr *sh, *newsh;

    size_t free = sdsavail(s);
    size_t len,newlen;

    if(free > addlen)
        return s;
    len = sdslen(s);

    sh = (void*)(s-(sizeof(struct sdshdr)));

    newlen = len + addlen;

    if(newlen < SDS_MAX_PREALLOC)
        newlen *= 2;
    else
        newlen += SDS_MAX_PREALLOC;

    newsh = realloc(sh,sizeof(struct sdshdr)+newlen+1);

    if (newsh == NULL) return NULL;

    newsh->free = newlen - len;

    return newsh->buf;


}
