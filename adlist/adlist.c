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

listIter *listGetIterator(list *list, int direction){
    listIter *iter;

    if ((iter = malloc(sizeof(struct listIter))) == NULL)
        return NULL;

    if (direction == AL_START_HEAD)
        iter->next = list->head;
    else
        iter->next = list->tail;

    iter->direction = direction;

    return iter;
}

listNode *listNext(listIter *iter){
    listNode * current;
    current = iter->next;

    if(current != NULL){
        //链表非空
        if(iter->direction == AL_START_HEAD)
            iter->next = current->next;
        else
            iter->next = current->prev;
        }

    return current;
}

void listReleaseIterator(listIter *iter){
    if(iter)
        free(iter);
}

void listRewind(list *list, listIter *iter){
    iter->next = list->head;
    iter->direction = AL_START_HEAD;
}

void listRewindTail(list *list, listIter *iter){
    iter->next = list->tail;
    iter->direction = AL_START_TAIL;
}

list *listDup(list *orig){
    list *copy;
    listIter *iter;
    listNode *node;

    if((copy = listCreate()) == NULL)
        return NULL;

    copy->match = orig->match;
    copy->free = orig->free;
    copy->dup = orig->dup;

    iter = listGetIterator(orig, AL_START_HEAD);

    while((node=listNext(iter)) != NULL){
        void *value;

        if(copy->dup){
            value = copy->dup(node->value);
            if(value == NULL){
                listReleaseIterator(iter);
                listRelease(copy);
                return NULL;
            }
        }else
            value = node->value;

        if(listAddNodeTail(copy, value) == NULL){
            listReleaseIterator(iter);
            listRelease(copy);
            return NULL;
        }
        
    }

    listReleaseIterator(iter);
    return copy;
}

listNode *listSearchKey(list *list, void *key){
    listNode * node;
    listIter * iter;

    iter = listGetIterator(list, AL_START_HEAD);

    while((node=listNext(iter)) != NULL){
        if(list->match){
            if(list->match(node->value, key)){
                listReleaseIterator(iter);
                return node;
            }
        }else{
            if(key == node->value){
                listReleaseIterator(iter);
                return node;
            }
        }
    }

    return NULL;
}

listNode *listIndex(list *list, long index){
    //支持从head和tail双向的,index小于0从tail开始,反之从head开始
    listNode *node;

    if(index<0){
        //负向从-1开始
        index = (-index-1);
        node = list->tail;

        while(index--&& node)
            node = node->prev;
    }else{
        //正向从0开始
        node = list->head;

        while(index--&&node)
            node = node->next;
    }
    return node;
}
