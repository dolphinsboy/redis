#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct sdshdr{
    int len;
    int free;
    char buf[];
}sdshdr;

int main(int argc, char **argv){

    struct sdshdr *sh;
    char *p = "Helloworld";

    sh = malloc(sizeof(struct sdshdr));

    sh->len = 10;
    sh->free = 0;

    memcpy(sh->buf, p, strlen(p));
    sh->buf[strlen(p)+1] = '\0';

    char *pt = (char*)sh->buf;

    struct sdshdr *psh = (void*)(pt-sizeof(struct sdshdr));
    printf("sizeof = %d\n", sizeof(sdshdr));

    //printf("len = %ld,free = %ld,buf = %ld, psh=%ld, c = %ld\n", &(psh->len), &(psh->free), &(psh->buf), psh, &(psh->c));
    printf("len = %ld,free = %ld,buf = %ld, psh=%ld\n", &(psh->len), &(psh->free), &(psh->buf), psh);
    printf("%d,%d,%s, %ld\n", psh->len, psh->free, psh->buf, psh);
    //printf("%d,%d,%d,%s, %ld\n", psh->len, psh->c, psh->free, psh->buf, psh);

    return 0;
}
