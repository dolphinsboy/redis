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
    printf("sds u en after update = %zu\n", sdslen(u));

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
    //sdsgrowzero(s, 10);
    //printf("after grow zero buf = %s, len = %zu, free = %zu\n", s, sdslen(s), sdsavail(s));

    char * ss = "Helloword";
    s = sdscatlen(s, ss, strlen(ss));
    printf("after sdscatlen buf = %s, len = %zu, free = %zu\n", s, sdslen(s), sdsavail(s));
    sdscat(s, copy);
    printf("after sdscat buf = %s, len = %zu, free = %zu\n", s, sdslen(s), sdsavail(s));

    u = sdscpylen(u, "FDDXEXDDGSDFGSEWREEE", 20);
    printf("after sdscpy buf = %s, len = %zu, free = %zu\n", u, sdslen(u), sdsavail(u));
    u = sdscpy(u, "DDDDDDDDDDDDDDDDDDDDDDDDDEEEEEEEEEEEEEEEEEEEEEEFFFFFFFFFFFFF");
    printf("after sdscpy buf = %s, len = %zu, free = %zu\n", u, sdslen(u), sdsavail(u));
    
    printFlag();
    s = sdsRemoveFreeSpace(s);
    printf("after remove space buf = %s, len = %zu, free = %zu\n", s, sdslen(s), sdsavail(s));

    char buf[21];
    sdsll2str(buf, -10);
    printf("sdsll2str = %s\n", buf);

    sdsull2str(buf,111);
    printf("sdsull2str = %s\n",buf);

    s = sdsfromlonglong(5349973883);
    printf("after fromlonglong buf = %s, len = %zu, free = %zu\n", s, sdslen(s), sdsavail(s));

    s = sdscatprintf(sdsempty(), "%s %d %c", "sdscatprintf", 22233, 'A');
    printf("after sdscatprintf buf = %s, len = %zu, free = %zu\n", s, sdslen(s), sdsavail(s));

    s = sdscatfmt(sdsempty(), "%s %S %i %I %u %U","AAA", u, -222, -3333, 222,111111222555111);
    printf("after sdscatfmt buf = %s, len = %zu, free = %zu\n", s, sdslen(s), sdsavail(s));

    sds t = sdsnew(" ddx . ddd . . . .                                   .");
    t = sdstrim(t, " . d ");
    printf("after sdstrim buf =%s, len = %zu, free = %zu\n", t, sdslen(t), sdsavail(t));

    sds r = sdsnew("Hello World"); 
    sdsrange(r, 100, 200);
    printf("after sdsrange buf =%s, len = %zu, free = %zu\n", r, sdslen(r), sdsavail(r));

    sds v  = sdsnew("Hello World");
    sdstolower(v);
    printf("after sdstolower buf =%s, len = %zu, free = %zu\n", v, sdslen(v), sdsavail(v));
    sdstoupper(v);
    printf("after sdstoupper buf =%s, len = %zu, free = %zu\n", v, sdslen(v), sdsavail(v));

    printf("cmp = %d\n", sdscmp(sdsnew("adb"), sdsnew("aDb")));

    printf("sizeofsds = %d\n", sizeof(sds));

    char *input = "A#_B#_C";
    int count;
    int len= strlen(input);
    sds*tokens;
    tokens = sdssplitlen(input, len , "#_",2, &count);

    int i;
    for(i = 0; i < count;i++){
        printf("%s\n",*(tokens+i));
    }

    printf("count=%d\n", count);

    sdsfree(copy);
    sdsfree(s);
    sdsfree(v);
    sdsfree(u);
    sdsfree(t);
    sdsfree(e);
    sdsfree(r);

    return 0;
}


