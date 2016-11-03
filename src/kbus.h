#ifndef _KBUS_H
#define _KBUS_H
#include"ktype.h"
#include"klist.h"
#include"kevent.h"
#include"kfifo.h"
#include "mesg.h"
#define MAX_PACKET 1024

typedef U8(*filter_mesg_callbck)(U32 type);
typedef struct _kbus{
    klist channel_list;
    //保护注册通道链表
    pthread_mutex_t* mtx;
}kbus;
typedef struct _kchannel{
    kbus* bus;
    klist  channel_node;
    kevent* ket;
   filter_mesg_callbck filter_mg;
    kfifo* msg_ff;
}kchannel;



extern kbus*  kbus_init();
extern I32 kbus_register_channel(kbus* bus,kchannel* channel);
extern I32 kbus_disregister_channel(kchannel* channel);
extern kchannel* kchannel_init(U32 fifo_size,I32 epoll_fd,filter_mesg_callbck fmc,kcallback fun);
extern void kchannel_mod_filter(kchannel* channel,filter_mesg_callbck fmc);
extern I32 kchannel_sendmsg(kchannel* channel,I8* buf);
extern I32 kchannel_recmesg(kchannel* channel,I8* buf);


#endif
