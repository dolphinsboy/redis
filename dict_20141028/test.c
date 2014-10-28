#include <stdio.h>
#include <stdlib.h>

void (*pfunction)(char *pbuf);

/*
 * 函数指针例子
 */
void print(char *pbuf)
{
    printf("function pointer:\n%s\n\n",pbuf);
}

/*
 * 指针数组
 */
void point_array(unsigned int row, unsigned int column)
{
    unsigned int **parray = malloc(sizeof(unsigned int*)*row);

    int idx=0;

    unsigned int **tmp_parray = parray;

    for(idx=0; idx<row; idx++)
    {
        unsigned int *p = malloc(sizeof(unsigned int)*column);

        *tmp_parray++ = p;

        int j=0;
        for(j=0; j<column; j++)
            *p++ = j+idx;
    }

    printf("parray[0][2]=%d\n",parray[2][2]);

    free(parray);
}

int main(int argc, char **argv)
{
    char *pbuf = "Hello world!";

    pfunction = print;

    pfunction(pbuf);

    point_array(5,10);

    return 0;
}
