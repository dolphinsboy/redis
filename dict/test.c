#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "dict.h"
#include "sds.h"

extern  int _dictKeyIndex(dict *d, void *key);

void print_dict(dict *d)
{
    printf("ht[0].size = %ld\nht[0].used = %ld\n",
            d->ht[0].size,
            d->ht[0].used);
}

int dictSdsKeyCaseCompare(void *privdata, const void *key1, const void*key2){
    DICT_NOTUSED(privdata);
    return strcasecmp(key1, key2) == 0;
}

int main(int argc, char **argv)
{
    dictType t = {
        dictHashFunction,
        NULL,
        NULL,
        //dictSdsKeyCaseCompare,
        NULL,
        NULL,
        NULL
    };

    sds s = sdsnew("test");

    dict *d = dictCreate(&t, NULL);

    dictExpand(d, 10);
    print_dict(d);
    dictAdd(d, "aa", "abbcxxxxxxxxxddd");
    print_dict(d);
    dictAdd(d, "bd", "abbc");
    print_dict(d);

    printf("sds=%s\n", s);
    printf("val=%s\n", dictFind(d, "aa")->v);

    //Seed部分
    struct timeval tv;
    gettimeofday(&tv,NULL);
    //seed为下面三者的异或操作
    printf("tv_sec=%d, tv_usec=%d pid=%d\n",tv.tv_sec, tv.tv_usec, getpid());
    dictSetHashFunctionSeed(tv.tv_sec^tv.tv_usec^getpid());
    printf("seed=%d\n", dictGetHashFunctionSeed());

    sdsfree(s);

    return 0;
}
