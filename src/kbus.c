#include"kbus.h"
#include<stdlib.h>
//总线、通道、消息类型过滤
kbus*  kbus_init()
{
    kbus* bus = (kbus*)malloc(sizeof(kbus));
    bus->mtx = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(bus->mtx,NULL);
    klist_init(&bus->channel_list);
    return bus;
}
I32 kbus_register_channel(kbus* bus,kchannel* channel)
{
    if(bus!=NULL)
    {
        channel->bus = bus;
        pthread_mutex_lock(channel->bus->mtx);
        klist_add_tail(&bus->channel_list,&channel->channel_node);
        pthread_mutex_unlock(channel->bus->mtx);
        return 0;
    }
    else
        return -1;
}
I32 kbus_disregister_channel(kchannel* channel)
{
    if(channel->bus!=NULL)
    {
        pthread_mutex_lock(channel->bus->mtx);
        klist_del(&channel->channel_node);
        pthread_mutex_unlock(channel->bus->mtx);
        return 0;
    }
    else
        return -1;
}
/*
* param1 消息队列大小
* param2 事件fd
* param3 过滤器回调函数
* param4 事件回调函数
*/
kchannel* kchannel_init(U32 fifo_size, I32 epoll_fd, filter_mesg_callbck fmc, kcallback fun)
{
    kchannel* channel = (kchannel*)malloc(sizeof(kchannel));
    channel->msg_ff = fifo_init(fifo_size);
    channel->ket = kevent_efd_new(epoll_fd,fun);
   channel->filter_mg  =fmc;
    return channel;
}


I32 kchannel_sendmsg(kchannel* channel,I8* buf)
{
    klist *pos1,*n1;
    kchannel* kc;
    Mesg* mg = (Mesg*)buf;
    pthread_mutex_lock(channel->bus->mtx);
    list_for_each_safe(pos1,n1,&channel->bus->channel_list)
    {
        kc = list_entry(pos1,kchannel,channel_node);
            if(kc->filter_mg==NULL || (*kc->filter_mg)(mg->type) == 1)
            {
                __fifo_put(kc->msg_ff,buf,sizeof( Mesg)+Mesg_len(buf));
                kevent_efd_notice(kc->ket);
        }
    }
    pthread_mutex_unlock(channel->bus->mtx);
    return 0;
}
I32 kchannel_recmesg(kchannel* channel,I8* buf)
{
    Mesg* mg = (Mesg*)buf;
    if(__fifo_get(channel->msg_ff,buf,sizeof( Mesg))!=sizeof( Mesg))
        goto label_err;
    if( __fifo_get(channel->msg_ff,buf+sizeof(Mesg),mg->len)!=mg->len)
        goto label_err;
    return 0;
label_err:
    return -1;
}
void kchannel_mod_filter(kchannel *channel, filter_mesg_callbck fmc)
{
    channel->filter_mg = fmc;
}


