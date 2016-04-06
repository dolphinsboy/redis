#include <stdlib.h>
#include "adlist.h"


list *listCreate(void)
{
    struct list *list;

    if((list = malloc(sizeof(*list))) == NULL)
        return NULL;

    list->head = list->tail = NULL;
    list->len = 0;
    list->dup = NULL;
    list->free = NULL;
    list->match = NULL;

    return list;
}

void listRelease(list *list)
{
    unsigned long len;
    listNode *current, *next;
    
    current = list->head;
    len = list->len;

    while(len--){
        next = current->next;
        if(list->free) list->free(current->value);
        free(current);
        current = next;
    }

    free(list);
}

list *listAddNodeHead(list *list, void *value)
{
    //将节点从头部插入
    listNode *node;

    if ((node = malloc(sizeof(listNode))) == NULL)
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

list *listAddNodeTail(list *list, void *value)
{
    //将节点从尾部插入
    listNode *node;

    if((node = malloc(sizeof(listNode))) == NULL)
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

list *listInsertNode(list *list, listNode *old_node, void *value, int after){
    listNode *node;

    if((node = malloc(sizeof(listNode))) == NULL)
        return NULL;
    node->value = value;

    if(after){
        //在原有节点后面插入
        //中间节点
        node->prev = old_node;
        node->next = old_node->next;

        if(list->tail == old_node)
            list->tail = node;
    }else{
        //在原有节点前面插入
        node->next = old_node;
        node->prev = old_node->prev;

        if(list->head == old_node)
            list->head = node;
    }

    if(node->prev != NULL)
        node->prev->next = node;

    if(node->next != NULL)
        node->next->prev = node;

    list->len++;
    return list;
}

void listDelNode(list *list, listNode *node)
{
    //是否为删除第一个节点
    if(node->prev){
        node->prev->next = node->next;
    }else{
        //删除第一个节点
        list->head = node->next;
    }

    //是否删除最后一个节点
    if(node->next){
        node->next->prev = node->prev;
    }else{
        //删除最后一个节点
        list->tail = node->prev;
    }
    free(node);
    list->len--;
}

listIter *listGetIterator(list *list, int direction)
{
    listIter *iter;

    if((iter = malloc(sizeof(listIter))) == NULL) return NULL;

    if(direction == AL_START_HEAD)
        iter->next = list->head;
    else
        iter->next = list->tail;
    iter->direction = direction;

    return iter;
}

void listReleaseIterator(listIter *iter){
    free(iter);
}

void listRewind(list *list, listIter *li){
    li->next = list->head;
    li->direction = AL_START_HEAD;
}

void listRewindTail(list *list, listIter *li){
    li->next = list->tail;
    li->direction = AL_START_TAIL;
}

listNode *listNext(listIter *iter)
{
    listNode *current = iter->next;

    if(current != NULL){
        if(iter->direction == AL_START_HEAD)
            iter->next = current->next;
        else
            iter->next = current->prev;
    }

    return current;
}
