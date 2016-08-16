#include "klist.h"
 void  klist_init( klist* head)
{
    head->next = head;
    head->prev = head;
}
//通用插入
 void  __list_add( klist *nd, klist *prev, klist *next)
{
    next->prev = nd;
    nd->next = next;
    nd->prev = prev;
    prev->next = nd;
}
//插入到表头
 void klist_add_head( klist* head, klist* nd)
{
    __list_add(nd,head,head->next);
}
//插入到表尾
 void klist_add_tail( klist* head, klist* nd)
{
    __list_add(nd,head->prev,head);
}
//通用删除
 void __list_del( klist* prev, klist* next)
{
    next->prev = prev;
    prev->next = next;
}
//封装通用删除 这样做的好处：防止空指针异常
 void  klist_del( klist *nd)
{
    __list_del(nd->prev,nd->next);
}
//从表头删除并返回
 klist* klist_del_head( klist *head)
{
     klist* tmp;

    if(head->next!=head)
     {
         tmp = head->next;
         klist_del(tmp);
    }
    else
        tmp = 0;
    return tmp;
}
//从表尾巴删除并返回
  klist* klist_del_tail( klist* head)
{
     klist* tmp;
    if(head->prev!=head)
    {
        tmp = head->prev;
        klist_del(tmp);
    }
    else
        tmp = 0;
    return tmp;
}
