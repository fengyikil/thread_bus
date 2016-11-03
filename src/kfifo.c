/*
 * 参考linux内核kfifo构建的fifo，如果只有一个生产者，一个消费者的情况下，
 * 不需要加锁，使用__fifo_get和__fifo_put即可。其他情况需要加锁，
 * 使用fifo_get和fifo_put.非Linux平台需要重新修改加锁机制。
 *
 * 作者：蒋俊杰
 * 最后修改：2015-3-26
*/
//#define __DEBUG__
#ifdef __DEBUG__
#define debug_msg(fmt,args...) do{\
    printf("[%s(%d)<%s>]\n",__FILE__,__LINE__,__FUNCTION__);\
    printf("\t\t\t");\
    printf((fmt),##args);\
}while(0)
#else
#define debug_msg(fmt,args...)
#endif
#include <malloc.h>
#include <stdio.h>
#include <pthread.h>
#include "kfifo.h"
//判断x是否是2的次方
#define is_power_of_2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))
//取a和b中最小值
#define min(a, b) (((a) < (b)) ? (a) : (b))
static inline U32 fls(U32 x)
{
    U32 position;
    U32 i;
    if(0 != x)
    {
        for (i = (x >> 1), position = 0; i != 0; ++position)
            i >>= 1;
    }
    else
    {
        position = -1;
    }
    return position+1;
}
static inline U32 roundup_pow_of_two(U32 x)
{
    return 1UL << fls(x - 1);
}
struct fifo* fifo_init(U32 size)
{
    struct fifo *f = NULL;
    if(!is_power_of_2(size))
    {
        debug_msg("size must be power of 2.\n");
        size = roundup_pow_of_two(size);
        debug_msg("size is be roundup to %d\n",size);
    }
    f = (struct fifo *)malloc(sizeof(struct fifo));
    if(!f)
    {
        debug_msg("malloc erro!\n");
        goto Lerr;
    }
    memset(f,0,sizeof(struct fifo));
    f->buffer = (unsigned char*)malloc(size);
    if(!f->buffer)
    {
        debug_msg("malloc erro!\n");
        goto Lerr;
    }
    f->mtx = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    if(!f->mtx)
    {
        debug_msg("malloc erro!\n");
        goto Lerr;
    }
    f->cond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
    if(!f->cond)
    {
        debug_msg("malloc erro!\n");
        goto Lerr;
    }
    if(pthread_mutex_init(f->mtx,NULL)!=0)
    {
        debug_msg("mutex init erro\n");
        goto Lerr;
    }
    if(pthread_cond_init(f->cond,NULL)!=0)
    {
        debug_msg("cond init erro\n");
        goto Lerr;
    }
    f->size = size;
    f->in = 0;
    f->out = 0;
    return f;
Lerr:
    if(f)
    {
        if(f->buffer)
            free(f->buffer);
        if(f->cond)
            free(f->cond);
        if(f->mtx)
            free(f->mtx);
        free(f);
    }
    return NULL;
}

void fifo_free(struct fifo* f)
{
    if(f)
    {
        if(f->buffer)
        {
            free(f->buffer);
            f->buffer = NULL;
        }
    if(f->mtx)
    {
        free(f->mtx);
        f->mtx = NULL;
    }
    if(f->cond)
    {
        free(f->cond);
        f->cond = NULL;
    }
        free(f);
        f = NULL;
    }
}
U32 __fifo_len(struct fifo* f)
{
    return (f->in - f->out);
}
void __fifo_reset(struct fifo *f)
{
    f->in = f->out = 0;
}
U32 __fifo_get(struct fifo* f,void *buf,U32 len)
{
    U32 l;
    unsigned char *buffer = buf;
    l = f->in - f->out;
    if(len > l)
    {
        debug_msg("fifo not enough len is %d\t fifo.len is %d!\n",len,(f->in - f->out));
        return (l);
    }
    else
    {
        l = min(len,f->size - (f->out & (f->size -1)));
        memcpy(buffer,f->buffer+(f->out & (f->size -1)),l);
        memcpy(buffer+l,f->buffer,len-l);
        f->out +=len;
        return len;
    }
}
U32 __fifo_put(struct fifo* f,void *buf,U32 len)
{
    U32 l;
    unsigned char *buffer = buf;
    l = f->size - f->in + f->out;
    if( len > l)
    {
        debug_msg("fifo have no such space!\t len is %d \t fifo.space is %d\n",len,l);
        return (l);
    }
    else
    {
        l = min(len,f->size - (f->in & (f->size -1)));
        memcpy(f->buffer+(f->in & (f->size -1)),buffer,l);
        memcpy(f->buffer,buffer+l,len-l);
        f->in += len;
        return len;
    }
}

U32 fifo_len(struct fifo *f)
{
    U32 llen=0;
    pthread_mutex_lock(f->mtx);
    llen = __fifo_len(f);
    pthread_mutex_unlock(f->mtx);
    return llen;
}
void fifo_reset(struct fifo *f)
{
        pthread_mutex_lock(f->mtx);
    f->in = f->out = 0;
    pthread_mutex_unlock(f->mtx);
}
U32 fifo_get(struct fifo *f,void *buffer,U32 len)
{
    U32 llen = 0;
    pthread_mutex_lock(f->mtx);
    while(__fifo_len(f)<len)
    {
        pthread_cond_wait(f->cond,f->mtx);
    }
   llen =  __fifo_get(f,buffer,len);
   pthread_mutex_unlock(f->mtx);
   return llen;
}
U32 fifo_put(struct fifo *f,void *buffer,U32 len)
{
    U32 llen = 0;
    pthread_mutex_lock(f->mtx);
    llen = __fifo_put(f,buffer,len);
    pthread_mutex_unlock(f->mtx);
    if(fifo_len(f) > len)
    {
        debug_msg("broadcast\n");
    pthread_cond_broadcast(f->cond);
    }
    else
    {
        debug_msg("signal\n");
    pthread_cond_signal(f->cond);
    }
    return llen;
}
//阻塞方式入队列，不成功则休眠1ms，再次尝试
void block_fifo_put(struct fifo *f,void *buffer,U32 len)
{
    while(fifo_put(f,buffer,len)!=len)
    {
       usleep(1000);
    }
}
