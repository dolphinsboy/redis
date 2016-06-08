#include <stdio.h>
#include <stdlib.h>
#include "sds.h"


int main(int argc, char **argv)
{
    sds s = sdsnew("test");
    printf("len = %zu\n", sdslen(s));

    return 0;
}


