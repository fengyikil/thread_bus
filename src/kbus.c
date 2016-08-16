#include"kbus.h"
#include"stdlib.h"
//总线、通道、消息类型过滤
kbus*  kbus_init()
{
    kbus* bus = (kbus*)malloc(sizeof(kbus));
    bus->mtx = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(bus->mtx,NULL);
    klist_init(&bus->channel_list);
    return bus;
}
int32 kbus_register_channel(kbus* bus,kchannel* channel)
{
    if(bus!=NULL)
    {
        channel->bus = bus;
//        pthread_mutex_lock(channel->bus->mtx);
        klist_add_tail(&bus->channel_list,&channel->channel_node);
//        pthread_mutex_unlock(channel->bus->mtx);
        return 0;
    }
    else
        return -1;
}
int32 kbus_disregister_channel(kchannel* channel)
{
    if(channel->bus!=NULL)
    {
//        pthread_mutex_lock(channel->bus->mtx);
        klist_del(&channel->channel_node);
//        pthread_mutex_unlock(channel->bus->mtx);
        return 0;
    }
    else
        return -1;
}

kchannel* kchannel_init(uint32 fifo_size,int32 epoll_fd,kcallback fun)
{
    kchannel* channel = (kchannel*)malloc(sizeof(kchannel));
    channel->msg_ff = fifo_init(fifo_size);
    channel->ket = kevent_efd_new(epoll_fd,fun);
    klist_init(&channel->msg_type_list);
    return channel;
}
int32 kchannel_add_msgtype(kchannel* channel,uint32 type)
{
    int ret = 0;
    msg_type* mt;
    klist *pos,*n;
    //去重
    list_for_each_safe(pos,n,&channel->msg_type_list)
    {
        mt = list_entry(pos,msg_type,msg_type_node);
        if(mt->type == type)
        {
            ret = -1;
        }
    }
    if(ret==0)
    {
        mt = (msg_type*)malloc(sizeof(msg_type));
        mt->type = type;
        klist_add_tail(&channel->msg_type_list,&mt->msg_type_node);
    }
    return ret;
}

int32 kchannel_remove_msgtype(kchannel* channel,uint32 type)
{
    int ret = -1;
    msg_type* mt;
    klist *pos,*n;
    list_for_each_safe(pos,n,&channel->msg_type_list)
    {
        mt = list_entry(pos,msg_type,msg_type_node);
        if(mt->type == type)
        {
            klist_del(&mt->msg_type_node);
            free(mt);
            ret = 0;
        }
    }
    return ret;
}
int32 kchannel_clear_msgtype(kchannel* channel)
{
    msg_type* mt;
    klist *pos,*n;
    list_for_each_safe(pos,n,&channel->msg_type_list)
    {
        mt = list_entry(pos,msg_type,msg_type_node);
        klist_del(&mt->msg_type_node);
        free(mt);
    }
    return 0;
}
int32 kchannel_sendmsg(kchannel* channel,int type,int len,char* data)
{
    klist *pos1,*n1,*pos2,*n2;
    kchannel* kc;
    msg_type* mt;
    kmesg_head khd;
    khd.msg_len = len;
    khd.msg_type = type;

//    pthread_mutex_lock(channel->bus->mtx);
    list_for_each_safe(pos1,n1,&channel->bus->channel_list)
    {
        kc = list_entry(pos1,kchannel,channel_node);
        list_for_each_safe(pos2,n2,&kc->msg_type_list)
        {
            mt = list_entry(pos2,msg_type,msg_type_node);
            if(mt->type==type)
            {
                __fifo_put(kc->msg_ff,&khd,sizeof( kmesg_head));
                __fifo_put(kc->msg_ff,data,len);
                kevent_efd_notice(kc->ket);
            }
        }
    }
//    pthread_mutex_unlock(channel->bus->mtx);
    return 0;
}
int32 kchannel_recmesg(kchannel* channel,kmesg* kg)
{
    if(__fifo_get(channel->msg_ff,&kg->msgh,sizeof( kmesg_head))!=sizeof( kmesg_head))
        goto label_err;
    if( __fifo_get(channel->msg_ff,kg->msg,kg->msgh.msg_len)!=kg->msgh.msg_len)
        goto label_err;
    return 0;
label_err:
    return -1;
}
kmesg* kmesg_init(int size)
{
    kmesg* kg = (kmesg*)malloc(sizeof(kmesg));
    kg->msg = malloc(size);
    return kg;
}
int32 kmesg_msgtype(kmesg* kg)
{
    return kg->msgh.msg_type;
}
int32 kmesg_msglen(kmesg* kg)
{
    return kg->msgh.msg_len;
}
void* kmesg_msg(kmesg* kg)
{
    return kg->msg;
}
