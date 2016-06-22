#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dict.h"

void print_dict(dict *d)
{
    printf("ht[0].size = %ld\nht[1].size = %ld\n",
            d->ht[0].size,
            d->ht[1].size);
}

int main(int argc, char **argv)
{
    dict *d = dictCreate(NULL, NULL);
    print_dict(d);

    dictExpand(d, 10);
    print_dict(d);

    return 0;
}
