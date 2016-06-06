#include <stdlib.h>
#include "adlist.h"

list* listCreate(void){
    struct list* list;
    
    if ( (list = malloc(sizeof(struct list))) == NULL )
        return NULL;

    list->head = list->tail = NULL;

    list->dup = NULL;
    list->match = NULL;
    list->free = NULL;

    list->len = 0;

    return list;
}

void listRelease(list* list){
   unsigned long len = list->len; 
   listNode *current, *next;

   current = list->head;
   while(len--){
        next = current->next;
        if(list->free) list->free(current->value);
        free(current);
        current = next;
   }

    free(list);
}

