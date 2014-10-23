#include <stdlib.h>
#include <string.h>
#include "adlist.h"

list *listCreate(void)
{
   struct list* plist;

   plist = malloc(sizeof(*plist));

   if(plist == NULL)
       return NULL;

   plist->head = plist->tail = NULL;

   plist->len = 0;

   plist->dup = NULL;
   plist->free = NULL;
   plist->match = NULL;

   return plist;
}

list *listAddNodeHead(list* plist, void *value)
{
    listNode * node;

    if((node=malloc(sizeof(*node))) == NULL)
        return NULL;

    node->value = value;

    size_t len = plist->len;

    if(len==0)
    {
        plist->head = node;
        plist->tail = node;

        node->prev = node->next = NULL;
    }else
    {
        node->prev = NULL;
        node->next = plist->head;
        plist->head->prev = node;
        plist->head = node;
    }

    plist->len++;

    return plist;
}

list *listAddNodeTail(list *plist, void *value)
{
    listNode *node;

    if( (node=malloc(sizeof(*node))) == NULL)
        return NULL;

    node->value = value;

    size_t len = plist->len;

    if(len == 0)
    {
        plist->head = node;
        plist->tail = node;

        node->prev = node->next = NULL;
    }else
    {
        node->prev = plist->tail;
        plist->tail->next = node;
        node->next = NULL;
        plist->tail = node;
    }

    plist->len++;

    return plist;
}

listIter *listGetIterator(list *plist, int direction)
{
    listIter *iter;

    if ((iter=malloc(sizeof(*iter))) == NULL)
            return NULL;

    if( direction == AL_START_HEAD )
        iter->next = plist->head;
    else
        iter->next = plist->tail;
    iter->direction = direction;

    return iter;
}

void listRelease(list *plist)
{
    struct listNode *current, *next;

    current = plist->head;

    next = current->next;

    unsigned long len = plist->len;
    while(len--)
    {
        next = current->next;
        if(plist->free) plist->free(current->value);

        free(current);
        current = next;
    }

    free(plist);
}
