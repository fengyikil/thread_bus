#ifndef  _MYLIST_
#define  _MYLIST_
//取父地址宏
#define container_of(ptr, type, member) ({                      \
         const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
          (type *)( (char *)__mptr - offsetof(type,member) );})
#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
//正向遍历宏
#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

typedef struct _klist
{
    struct _klist *prev,*next;
}klist;

extern void  klist_init( klist* head);
extern void klist_add_head( klist* head, klist* nd);
extern void klist_add_tail( klist* head, klist* nd);
extern void  klist_del( klist *nd);
extern klist* klist_del_head( klist *head);
extern klist* klist_del_tail( klist* head);

#endif
