#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sds.h"

void printFlag(){
    printf("#######################\n\n");
}

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

    sdsclear(u);

    printf("after clear len = %zu, free = %zu\n", sdslen(u), sdsavail(u));
    
    s = sdsMakeRoomFor(s, 1024);
    printf("after make room buf = %s, len = %zu, free = %zu\n", s, sdslen(s), sdsavail(s));
    printf("sdsAllocSize=%zu\n", sdsAllocSize(s));
    
    //printFlag();
    //sdsIncrLen(s, 10);
    //printf("after incr 10 len buf = %s, len = %zu, free = %zu\n", s, sdslen(s), sdsavail(s));
    sdsIncrLen(s, -2);
    printf("after incr -2 len buf = %s, len = %zu, free = %zu\n", s, sdslen(s), sdsavail(s));
    printFlag();
    sdsgrowzero(s, 10);
    printf("after grow zero buf = %s, len = %zu, free = %zu\n", s, sdslen(s), sdsavail(s));
    
    printFlag();
    s = sdsRemoveFreeSpace(s);
    printf("after remove space buf = %s, len = %zu, free = %zu\n", s, sdslen(s), sdsavail(s));
    
    sdsfree(copy);
    sdsfree(s);
    sdsfree(e);

    return 0;
}


