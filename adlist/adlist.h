#ifndef __AD_LIST_H
#define __AD_LIST_H

typedef struct listNode{
    //前置节点
    struct listNode * prev;
    //后置节点
    struct listNode * next;

    //节点值
    void * value;
} listNode;

//节点迭代器
typedef struct listIter {
    listNode * next;
    //迭代方向
    int direction;
} listIter;

typedef struct list{
    //链表表头
    listNode * head;
    //链表结尾
    listNode * tail;
    //复制一个节点
    void(*dup)(void *ptr);
    void(*free)(void *ptr);
    //节点match函数
    int(*match)(void * ptr, void *key);
    //链表长度
    unsigned long len;
}list;
//常用的宏函数
#define listLength(l) ((l)->len)
#define listFirst(l) ((l)->head)
#define listLast(l) ((l)->tail)
#define listPrevNode(n) ((n)->prev)
#define listNextNode(n) ((n)->next)
#define listNodeValue(n) ((n)->value)

#define listSetDupMethod(l, m) ((l)->dup = (m)) 
#define listSetMatchMethod(l, m) ((l)->match = (m))
#define listSetFreeMethod(l, m) ((l)->free = (m))

#define listGetDupMethod(l) ((l)->dup)
#define listGetFree(l) ((l)->free)
#define listGetMatch(l) ((l)->match)

#define AL_DIRECTION_HEAD 0
#define AL_DIRECTION_TAIL 1

//创建链表
list * listCreate(void);
//释放链表
void listRelease(list *list);
//链表头部添加一个节点
list *listAddNodeHead(list *list, void *value);
//链表尾部添加一个节点 
list *listAddNodeTail(list *list, void *value);
//在指定节点的位置添加新节点
list *listInsertNode(list *list, listNode*old_node, void *value, int after);
//删除指定节点
void listDelete(list *list, listNode *node);
//获取链表的迭代器
listIter *listGetIterator(list *list, int direction);
#endif
