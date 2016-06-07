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

list *listAddNodeHead(list *list, void *value){
    listNode * node;

    if ((node = malloc(sizeof(struct listNode))) == NULL)
        return NULL;
    node->value = value;

    if(list->len == 0){
        list->head = list->tail = node;
        node->prev = node->next = NULL;
    }else{
        node->prev = NULL;
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
    }

    list->len++;
    return list;
}

list* listAddNodeTail(list *list, void *value){
    listNode * node;

    if((node = malloc(sizeof(*node))) == NULL)
        return NULL;
    node->value = value;

    if(list->len == 0){
        list->head = list->tail = node;
        node->prev = node->next = NULL;
    }else{
        node->next = NULL;
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
    }

    list->len++;
    return list;
}

list *listInsertNode(list *list, listNode * old_node, void *value, int after){
    listNode * node;

    if ((node = malloc(sizeof(*node))) == NULL)
        return NULL;
    node->value = value;

    if(after){
       
        node->prev = old_node;
        node->next = old_node->next;

        if(list->tail == old_node){
            list->tail = node;
        }else{
            old_node->next->prev = node;
        }
        old_node->next = node;

    }else{
        node->next = old_node;
        node->prev = old_node->prev;

        if(list->head == old_node){
            list->head = node;
        }else{
            old_node->prev->next = node;
        }
        old_node->prev = node;
    }
    
    list->len++;
    return list;
}

void listDelete(list *list, listNode * node){
    //通过节点的prev和next指定来区分删除的是尾部,头部还是中部 
    if(node->prev)
        node->prev->next = node->next;
    else
        list->head = node->next;

    if(node->next)
        node->next->prev = node->prev;
    else
        list->tail = node->prev;

    free(node);
    list->len--;
}
