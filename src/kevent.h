#ifndef      _KEVENT_H
#define     _KEVENT_H
#include "ktype.h"

#define MAX_EVENTS 255
typedef struct _kevent kevent;
typedef void(*kcallback)( kevent* );

struct _kevent
{
     //epoll支持的fd,如socket 、eventfd、timerfd
    I32 fd;
    //挂靠的主epollfd
    I32 epoll_fd;
    //事件回调函数
    kcallback fun;
};

extern void kevent_write( kevent *,I32 msg);
extern I32 kevent_read( kevent* ket);

//主框架
extern void kevent_null_callback( kevent *);
extern I32 kevent_create_epollfd();
extern void kevent_loop(I32 epollfd);
//通用的fd
extern  kevent*  kevent_new(int epoll_fd,int fd,kcallback fun);
//extern INTEGER32  kevent_bind( kevent* ket);
extern I32 kevent_break( kevent* ket);
//eventfd
extern  kevent* kevent_efd_new(int epoll_fd,kcallback fun);
extern void kevent_efd_delete( kevent* ket);
//timefd
extern  kevent* kevent_time_new(int epoll_fd,kcallback fun,int ms, int iscycle);
extern I32 kevent_time_stop( kevent* ket);
extern int kevent_time_reset( kevent* ket,kcallback fun,I32 ms,I32 iscycle);
extern void kevent_time_delete( kevent* ket);
extern void kevent_time_free( kevent* ket);
extern I32 kevent_time_isfree( kevent* ket);
#endif
