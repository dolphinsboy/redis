#include <stdio.h>
#include <stdlib.h>
#include "adlist.h"

int main(){
    listNode * node = malloc(sizeof(listNode));
    node->prev = NULL;
    node->next = NULL;
    node->value = "test";

    printf("node->value=%s\n", node->value);

    free(node);

    list *list = listCreate();

    printf("len = %d\n", list->len);

    listAddNodeHead(list, "A");
    listAddNodeHead(list, "B");
    listAddNodeTail(list, "D");
    listAddNodeTail(list, "C");

    printf("len = %d\n", list->len);

    unsigned long len = list->len;
    listNode * current,*next;

    current = list->head;

    while(len--){
        next = current->next;
        printf("node->value=%s\n", current->value);
        current = next;
    }
    listRelease(list);

    return 0;
}
