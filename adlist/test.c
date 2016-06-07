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

    listInsertNode(list, list->head, "C", 1);
    listInsertNode(list, list->tail, "D", 1);
    listInsertNode(list, list->head, "F", 0);
    listInsertNode(list, list->tail, "E", 0);

    listDelete(list, list->tail);

    printf("len = %d\n", list->len);

    unsigned long len = list->len;

    //从head向tail遍历
    printf("########head->tail##########\n");
    listNode * current,*next;
    current = list->head;
    while(len--){
        next = current->next;
        printf("node->value=%s\n", current->value);
        current = next;
    }

    //从tail向head遍历
    
    printf("########tail->head##########\n");
    len = list->len;
    current = list->tail;
    listNode *prev;

    while(len--){
        prev = current->prev;
        printf("node->value=%s\n", current->value);
        current = prev;
    }

    struct list *copy;
    copy = listDup(list);

    listIter * iter = listGetIterator(copy,0);
    printf("direction=%d, value=%s\n", iter->direction, iter->next->value);

    iter = listGetIterator(copy,1);
    printf("direction=%d, value=%s\n", iter->direction, iter->next->value);
    listRelease(list);

    return 0;
}
