#ifndef H_CONNECTION
#define H_CONNECTION

#include "circularbuffer.h"


/* It must be power-of-2, 2 ** n
 */
#define BUFFERSIZE  1024
#define MAXCONNECTIONS  10

enum connectiontype {  
    CNTYPE_TCP,
}; 


struct connection{
    struct ringbuffer outbuffer;
    int sockfd;
    enum connectiontype type;
    struct sockaddr_in *address;
    int slot;
};


int tcpconnection_accept(int epollfd, int listenfd);
void connection_broadcast(const char *buff, int len);

#endif
