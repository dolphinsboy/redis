#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "adlist.h"

int main(int argc, char **argv)
{
    struct list* list;
    listNode *node;

    list = listCreate();

    printf("len = %ld\n", listLength(list));

    list = listAddNodeHead(list, "A");
    list = listAddNodeHead(list, "B");
    list = listAddNodeHead(list, "C");
    list = listAddNodeTail(list, "D");

    printf("len = %ld\n", listLength(list));

    /*node = list->tail;
    while(node != NULL){
        printf("NodeValue = %s\n", listNodeValue(node));

        node = node->prev;
    }*/
    listIter *iter = listGetIterator(list, 0);
    while((node = listNext(iter)) != NULL){
        printf("NodeValue=%s\n", listNodeValue(node));
    }

    listRelease(list);


    return 0;
}
