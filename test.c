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

  kmesg* kg1;
  kmesg* kg2;
  kmesg* kg3;

  kevent* time1;

struct test{
      char b;
      int a;
      float c;
  };


 void bus_event_fun1( kevent* ket)
 {
     //防止不断触发
     kevent_void_repeat(ket);
      struct test* tt;
     while( kchannel_recmesg(kc1,kg1)!=-1)
     {
      tt =(struct test*) kmesg_msg(kg1);
      printf("pid1 is %u\t msgtype is %d\n",pthread_self(),kmesg_msgtype(kg1));
     }
 }
 void bus_event_fun2(kevent* ket)
 {

     //防止不断触发
     kevent_void_repeat(ket);
      struct test* tt;
      while( kchannel_recmesg(kc2,kg2)!=-1)
      {
        tt = (struct test*)kmesg_msg(kg2);
      printf("pid2 is %u\t msgtype is %d\n",pthread_self(),kmesg_msgtype(kg2));
     }
      kchannel_remove_msgtype(kc2,8);
 }
 void bus_event_fun3(kevent* ket)
 {
     //防止不断触发
     kevent_void_repeat(ket);
      struct test* tt;
      while( kchannel_recmesg(kc3,kg3)!=-1)
      {
      tt = (struct test*)kmesg_msg(kg3);
      printf("pid3 is %u\t msgtype is %d\n",pthread_self(),kmesg_msgtype(kg3));
     }
 }

void time1_fun(kevent* ket)
{
    kevent_void_repeat(ket);

static struct test tt={3,2,9.8};
tt.a ++;

int b = 8;
kchannel_sendmsg(kc3,9,sizeof(tt),(char*)&tt);
kchannel_sendmsg(kc1,8,sizeof(b),(char*)&b);
printf("\n");
}

void* td1_thread(void* arg)
{
    printf("t1_thread\n");
    int epollfd = kevent_create_epollfd();
    kg1 = kmesg_init(100);
    kc1 = kchannel_init(1000,epollfd,bus_event_fun1);
    kbus_register_channel(bus,kc1);
    kchannel_add_msgtype(kc1,9);


     time1 = kevent_time_new(epollfd,time1_fun,1000,1);
     kevent_loop(epollfd);

}
void* td2_thread(void* arg)
{
      printf("t2_thread\n");
    int epollfd = kevent_create_epollfd();
   kg2 = kmesg_init(100);
   kc2 = kchannel_init(1000,epollfd,bus_event_fun2);
   kbus_register_channel(bus,kc2);
   kchannel_add_msgtype(kc2,9);
   kchannel_add_msgtype(kc2,8);

   kevent_loop(epollfd);

}
void* td3_thread(void* arg)
{
      printf("t3_thread\n");
    int epollfd = kevent_create_epollfd();
    kg3 = kmesg_init(100);
    kc3 = kchannel_init(1000,epollfd,bus_event_fun3);
    kbus_register_channel(bus,kc3);
//    kchannel_add_msgtype(kc3,9);

    kevent_loop(epollfd);

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
