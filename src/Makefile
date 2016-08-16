CC = gcc
CFLAGS = -Wall -fPIC 
LDFLAGS = -lpthread
INCLUDES = -I.

OBJECTS = kbus.o kevent.o kfifo.o klist.o

all : ${OBJECTS}
	ar cr libkbus.a ${OBJECTS}
clean :
	rm -rf libkbus.a ${OBJECTS}

#dependence
%.o : %.c
	${CC} ${CFLAGS} -c $< -o $@ 
 
