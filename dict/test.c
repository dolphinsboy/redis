#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    sdsfree(s);

    return 0;
}
