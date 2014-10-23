#ifndef __ADLIST_H
#define __ADLIST_H

typedef struct listNode{
    //前驱节点
    struct listNode *prev;

    //后继节点
    struct listNode *next;

    //值
    void *value;
} listNode;

typedef struct list{
    //表头指针
    listNode *head;

    //表尾指针
    listNode *tail;

    //节点数目
    unsigned long len;

    //复制函数
    void *(*dup)(void *ptr);

    //释放函数
    void (*free)(void *ptr);

    //比对函数
    int (*match)(void *ptr, void *key);
}list;

typedef struct listIter{
    //下一个节点
    struct listNode *next;

    //迭代方向
    int direction;
}listIter;

#define listLength(l) ((l)->len)

list *listCreate(void);
list *listAddNodeHead(list *plist, void *value);
list *listAddNodeTail(list *plist, void *value);
listIter *listGetIterator(list *plist, int direction);
void listRelease(list *plist);

#define AL_START_HEAD 0
#define AL_START_TAIL 1
#endif
