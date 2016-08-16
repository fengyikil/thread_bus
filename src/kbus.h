#ifndef _KBUS_H
#define _KBUS_H
#include"ktype.h"
#include"klist.h"
#include"kevent.h"
#include"kfifo.h"
typedef struct _kbus{
    klist channel_list;
    //保护注册通道链表
    pthread_mutex_t* mtx;
}kbus;
typedef struct _kchannel{
    kbus* bus;
    klist  channel_node;
    kevent* ket;
    klist msg_type_list;
    kfifo* msg_ff;
}kchannel;
typedef struct _msg_type{
    klist msg_type_node;
    uint32 type;
}msg_type;
typedef struct _kmesg_head{
    uint32 msg_type;
    uint32 msg_len;
}kmesg_head;
typedef struct _kmesg{
    kmesg_head msgh;
    uint8* msg;
}kmesg;

extern kbus*  kbus_init();
extern int32 kbus_register_channel(kbus* bus,kchannel* channel);
extern int32 kbus_disregister_channel(kchannel* channel);
extern kchannel* kchannel_init(uint32 fifo_size,int32 epoll_fd,kcallback fun);
extern int32 kchannel_add_msgtype(kchannel* channel,uint32 type);
extern int32 kchannel_remove_msgtype(kchannel* channel,uint32 type);
extern int32 kchannel_clear_msgtype(kchannel* channel);
extern int32 kchannel_sendmsg(kchannel* channel,int type,int len,char* data);
extern int32 kchannel_recmesg(kchannel* channel,kmesg* kg);
extern kmesg* kmesg_init(int size);
extern int32 kmesg_msgtype(kmesg* kg);
extern int32 kmesg_msglen(kmesg* kg);
extern void* kmesg_msg(kmesg* kg);

#endif
