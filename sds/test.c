#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sds.h"

int main(int argc, char **argv){
    sds s = sdsnew("test");
    sds e = sdsempty();
    sds copy = sdsdup(s);

    printf("len = %zu\n", sdslen(s));
    printf("empty = %zu\n", sdslen(e));
    printf("copy = %s\n", copy);

    sds u = sdsnew("xxxxxxxxxxxxxxxxxxx");
    printf("len before update = %zu\n", sdslen(u));
    u[5]='\0';
    sdsupdatelen(u);
    printf("len after update = %zu\n", sdslen(u));

    sdsfree(copy);
    sdsfree(s);
    sdsfree(e);

    return 0;
}


