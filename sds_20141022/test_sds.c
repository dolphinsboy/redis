#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdarg.h>

#include "sds.h"

int main(int argc, char** argv)
{
    sds pbuf = "Helloworld!\n";

    sds new_buf = sdsMakeRoomFor(pbuf,1040);

    printf("%s\n",new_buf);

    printf("sizeof=%d",sizeof(new_buf));


    return 0;
}

