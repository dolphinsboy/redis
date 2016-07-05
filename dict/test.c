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

int main(int argc, char **argv)
{
    dictType *t = NULL;

    sds s = sdsnew("test");

    if((t = malloc(sizeof(dictType))) == NULL)
        return -1;
    t->hashFunction = &dictHashFunction;
    dict *d = dictCreate(t, NULL);
    print_dict(d);

    dictExpand(d, 10);
    print_dict(d);

    dictAdd(d, "a", "abbc");
    dictAdd(d, "b", "abbc");

    printf("idx=%d\n", _dictKeyIndex(d, "a"));
    printf("idx=%d\n", _dictKeyIndex(d, "b"));
    printf("idx=%d\n", _dictKeyIndex(d, "c"));

    printf("sds=%s\n", s);

    sdsfree(s);

    return 0;
}
