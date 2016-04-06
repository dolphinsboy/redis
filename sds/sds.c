#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "sds.h"

sds sdsnewlen(const char*init, size_t initlen){
    struct sdshdr *sh;

    if(init){
        sh = malloc(sizeof(struct sdshdr) + initlen + 1);
    }else{
        sh = malloc(sizeof(struct sdshdr) + initlen + 1);
    }

    if(sh == NULL) return NULL;

    sh->len = initlen;
    sh->free = 0;

    if(init && initlen)
        memcpy(sh->buf, init, initlen);
    sh->buf[initlen] = '\0';

    return (char*)sh->buf;
}

sds sdsnew(const char* init){
    size_t initlen = (init== NULL) ? 0 : strlen(init);
    return sdsnewlen(init, initlen);
}
