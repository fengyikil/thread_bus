#include <stdio.h>
#include <sys/types.h>
#include <error.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <sys/eventfd.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#include"kbus.h"
kbus* bus;

kchannel* kc1;
kchannel* kc2;
kchannel* kc3;

kevent* time1;

//2种不同类型消息
struct type_msg1{
    int a;
    int b;
};
struct type_msg2{
    char a;
    int b;
    float c;
};
//3个线程总线消息过滤函数 、处理函数

//1
unsigned char  thread1_filter_mesg_callbck(unsigned int type)
{
    //接收所有消息（Receive all messages）
    return 1;
}
void thread1_bus_event( kevent* ket)
{
    //防止不断触发
    kevent_read(ket);
    char buf[50];
    struct type_msg1* m1;
    struct type_msg2* m2;
    while( kchannel_recmesg(kc1,buf)!=-1)
    {
        if(Mesg_type(buf)==1)
        {
            m1 = Mesg_data(buf);
            printf("pid1 is %u\t msgtype is %d\t m1->a is %d\n",pthread_self(),Mesg_type(buf),m1->a);

        }
        else if(Mesg_type(buf)==2)
        {
            m2 = Mesg_data(buf);
            printf("pid1 is %u\t msgtype is %d\t m2->c is %f\n",pthread_self(),Mesg_type(buf),m2->c);
        }
    }
    printf("\n");
}
//2
unsigned char  thread2_filter_mesg_callbck(unsigned int type)
{
    //只接收 1号消息（Receive all messages）
    if(type==1)
            return 1;
    else
    return 0;
}
void thread2_bus_event(kevent* ket)
{

    //防止不断触发
    kevent_read(ket);
    char buf[50];
    struct type_msg1* m1;
    while( kchannel_recmesg(kc2,buf)!=-1)
    {
        m1 = Mesg_data(buf);
        printf("pid2 is %u\t msgtype is %d\t m1->a is %d\n",pthread_self(),Mesg_type(buf),m1->a);
    }
    printf("\n");
}
//3
unsigned char  thread3_filter_mesg_callbck(unsigned int type)
{
    //只接收2号消息（Receive all messages）
    if(type==2)
            return 1;
    else
    return 0;
}
void thread3_bus_event(kevent* ket)
{
    //防止不断触发
    kevent_read(ket);
    char buf[50];
    struct type_msg2* m2;
    while( kchannel_recmesg(kc3,buf)!=-1)
    {
        m2 = Mesg_data(buf);
        printf("pid3 is %u\t msgtype is %d\t m2->c is %f\n",pthread_self(),Mesg_type(buf),m2->c);
    }
    printf("\n");
}

//定时发送三种消息
void time1_fun(kevent* ket)
{
    kevent_read(ket);
    char buf[50];
    static struct type_msg1 m1 ={3,4};
    static struct type_msg2 m2 ={1,3,4.3};
    m1.a ++;
    m2.c = m2.c + 0.1;
    //建立消息到buf，并发送
    Mesg_build(1,sizeof(m1),(char*)&m1,buf);
    kchannel_sendmsg(kc1,buf);

    Mesg_build(2,sizeof(m2),(char*)&m2,buf);
    kchannel_sendmsg(kc1,buf);

    printf("\n");
}

void* td1_thread(void* arg)
{
    printf("t1_thread\n");
    int epoll_fd = kevent_create_epollfd();
    kc1 = kchannel_init(1000,epoll_fd,thread1_filter_mesg_callbck,thread1_bus_event);
    kbus_register_channel(bus,kc1);


    time1 = kevent_time_new(epoll_fd,time1_fun,1000,1);

    kevent_loop(epoll_fd);

}
void* td2_thread(void* arg)
{
    printf("t2_thread\n");
    int epoll_fd = kevent_create_epollfd();
    kc2 = kchannel_init(1000,epoll_fd,thread2_filter_mesg_callbck,thread2_bus_event);
    kbus_register_channel(bus,kc2);

    kevent_loop(epoll_fd);

}
void* td3_thread(void* arg)
{
    printf("t3_thread\n");
    int epoll_fd = kevent_create_epollfd();
    kc3 = kchannel_init(1000,epoll_fd,thread3_filter_mesg_callbck,thread3_bus_event);
    kbus_register_channel(bus,kc3);

    kevent_loop(epoll_fd);

}

int main(int argc, char *argv[])
{
    pthread_t    t1_t;
    pthread_t    t2_t;
    pthread_t    t3_t;
    bus = kbus_init();
    pthread_create(&t1_t,NULL,td1_thread,NULL);
    pthread_create(&t2_t,NULL,td2_thread,NULL);
    pthread_create(&t3_t,NULL,td3_thread,NULL);


    pthread_join(t1_t,NULL);
    pthread_join(t2_t,NULL);
    pthread_join(t3_t,NULL);

    return 0;
}
