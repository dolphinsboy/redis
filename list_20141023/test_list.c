#include <stdio.h>
#include <stdlib.h>
#include "adlist.h"

int main(int argc, char **argv)
{
    size_t a[] = {1,2,3,4,5,6,7,8,9};

    size_t idx;
    size_t len = sizeof(a)/sizeof(size_t);


    struct list* plist = listCreate();

    //初始化双端链表数据
    for(idx=0;idx<len;idx++)
    {
        if(idx%2 == 0)
            plist = listAddNodeHead(plist,&a[idx]);
        else
            plist = listAddNodeTail(plist,&a[idx]);
    }

    printf("List_len=%d\n",listLength(plist));

    //遍历链表
    listNode *iter = plist->head;

    while(iter!=NULL)
    {
        int *value =(int *)iter->value;
        printf("%d\n",*value);
        iter = iter->next;
    }

    listRelease(plist);

    printf("Release_done")

    return 0;
}
