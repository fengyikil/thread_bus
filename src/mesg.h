#ifndef          _MESG_H_
#define        _MESG_H_
#define MASK 0x68686868
typedef struct _Mesg
{
    unsigned int mask;
    unsigned int  type;
    unsigned int len;
}Mesg;

char* Mesg_build(unsigned int type,unsigned int  len,char* data,char* buf);
unsigned int Mesg_len(char* buf);
unsigned int Mesg_type(char* buf);
void* Mesg_data(char* buf);
#endif
