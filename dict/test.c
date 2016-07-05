#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dict.h"
#include "sds.h"

void print_dict(dict *d)
{
    printf("ht[0].size = %ld\nht[1].size = %ld\n",
            d->ht[0].size,
            d->ht[1].size);
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
    print_dict(d);

    dictExpand(d, 10);
    print_dict(d);

    dictAdd(d, "aa", "abbc");
    dictAdd(d, "b", "abbc");

    printf("%s idx=%d\n", "aa", _dictKeyIndex(d, "aa"));
    printf("%s idx=%d\n", "b", _dictKeyIndex(d, "b"));
    printf("%s idx=%d\n", "c", _dictKeyIndex(d, "c"));
    printf("%s idx=%d\n", "a", _dictKeyIndex(d, "a"));
    printf("%s idx=%d\n", "d", _dictKeyIndex(d, "d"));

    printf("sds=%s\n", s);

    sdsfree(s);

    return 0;
}
