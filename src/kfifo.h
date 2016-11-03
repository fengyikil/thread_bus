#ifndef __FIFO_H__
#define __FIFO_H__
#include<pthread.h>
#include"ktype.h"
//循环队列
typedef struct fifo
{
    U8  *buffer;
    U32  size;
    U32  in;
    U32 out;
    pthread_mutex_t *mtx;
    pthread_cond_t *cond;
}kfifo;

extern struct fifo* fifo_init(U32 size);
extern void fifo_free(struct fifo* f);
extern void __fifo_reset(struct fifo *f);
extern U32 __fifo_get(struct fifo* f,void *buf,U32 len);
extern U32 __fifo_put(struct fifo* f,void *buf,U32 len);
extern U32 fifo_len(struct fifo *f);
extern void fifo_reset(struct fifo *f);
extern U32 fifo_get(struct fifo *f,void *buffer,U32 len);
extern U32 fifo_put(struct fifo *f,void *buffer,U32 len);
extern void block_fifo_put(struct fifo *f,void *buffer,U32 len);
#endif
