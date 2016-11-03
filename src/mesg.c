#include "mesg.h"
#include <string.h>
char* Mesg_build(unsigned int type, unsigned int len, char* data, char* buf)
{
    Mesg* mg = (Mesg*)buf;
    mg->mask =MASK;
    mg->type = type;
    mg->len   = len;
    if(len>0)
    memcpy(buf+sizeof(Mesg),data,len);
    return buf;
}

unsigned int Mesg_len(char *buf)
{
    Mesg* mg = (Mesg*)buf;
    return mg->len;
}

unsigned int Mesg_type(char *buf)
{
    Mesg* mg = (Mesg*)buf;
    return mg->type;
}

void* Mesg_data(char *buf)
{
    return buf+sizeof(Mesg);
}
