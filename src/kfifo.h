#ifndef __FIFO_H__
#define __FIFO_H__
#include<pthread.h>
#include"ktype.h"
//循环队列
typedef struct fifo
{
    uint8  *buffer;
    uint32  size;
    uint32  in;
    uint32 out;
    pthread_mutex_t *mtx;
    pthread_cond_t *cond;
}kfifo;

extern struct fifo* fifo_init(uint32 size);
extern void fifo_free(struct fifo* f);
extern void __fifo_reset(struct fifo *f);
extern uint32 __fifo_get(struct fifo* f,void *buf,uint32 len);
extern uint32 __fifo_put(struct fifo* f,void *buf,uint32 len);
extern uint32 fifo_len(struct fifo *f);
extern void fifo_reset(struct fifo *f);
extern uint32 fifo_get(struct fifo *f,void *buffer,uint32 len);
extern uint32 fifo_put(struct fifo *f,void *buffer,uint32 len);
extern void block_fifo_put(struct fifo *f,void *buffer,uint32 len);
#endif
