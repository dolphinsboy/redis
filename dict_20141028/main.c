#include <stdio.h>
#include <stdlib.h>
#include "dict.h"

int main(int argc, char **argv)
{
    dictType *dtype = malloc(sizeof(*dtype));
    dtype->hashFunction = NULL;

    void *privDataPtr = NULL;

    dict * d = dictCreate(dtype,privDataPtr);

    printf("iterators:%d\n",d->iterators);
    return 0;
}
