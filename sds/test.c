#include <stdio.h>
#include <stdlib.h>
#include "sds.h"


int main(int argc, char **argv)
{
    sds p = sdsnew("Helloworld");

    printf("len = %zu, free = %zu\n", sdslen(p), sdsavail(p));

    return 0;
}


