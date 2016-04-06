#ifndef __ADLIST_H
#define __ADLIST_H

typedef struct listNode{
    //前置节点
    struct listNode *prev;

    //后置节点
    struct listNode *next;

    //节点的值
    void *value;
}listNode;

typedef struct listIter{
    listNode *next;
    int direction;
}listIter;


typedef struct list{
    //表头节点
    listNode *head;

    //表尾节点
    listNode *tail;

    //链表所包含的节点数量
    unsigned long len;

    //节点值复制函数
    void *(*dup)(void *ptr);

    //节点值释放函数
    void (*free)(void *ptr);

    //节点值对比函数
    int (*match)(void *ptr, void *key);

}list;
/*Functions implemented as macros*/
#define listLength(l) ((l)->len)
#define listFirst(l) ((l)->head)
#define listLast(l) ((l)->last)
#define listPrevNode(n) ((n)->prev)
#define listNexNode(n) ((n)->next)
#define listNodeValue(n) ((n)->value)

#define listSetDupMethod(l,m) ((l)->dup = (m))
#define listSetFreeMethod(l, m) ((l)->free = (m))
#define listSetMatchMethod(l,m) ((l)->match = (m))

#define listGetDupMethod(l) ((l)->dup)
#define listGetFree(l) ((l)->free)
#define listGetMatchMethod(l) ((l)->match)

/*Prototypes*/

list *listCreate(void);
void listRelease(list *list);
list *listAddNodeHead(list *list, void *value);
list *listAddNodeTail(list *list, void *value);
list *listInsertNode(list *list, listNode *old_node, void *value, int after);
void listDelNode(list *list, listNode*node);
listIter *listGetIterator(list *list, int direction);
void listReleaseIterator(listIter *iter);
list *listDup(list *orig);
listNode *listSearchKey(list *list, void *key);
listNode *listIndex(list *list, long index);
void listRewind(list *list, listIter*li);
void listRewind(list *list, listIter *li);
void listRotate(list *list);

/*Directions for iterators*/
#define AL_START_HEAD 0
#define AL_START_TAIL 1

#endif
