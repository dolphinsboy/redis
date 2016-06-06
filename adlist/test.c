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

    listRelease(list);

    return 0;
}
