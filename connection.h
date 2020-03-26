#ifndef H_CONNECTION
#define H_CONNECTION

#include "circularbuffer.h"


enum connectiontype {  
    CNTYPE_TCP,
}; 


struct connection{
    int sockfd;
    enum connectiontype type;
    struct sockaddr_in *address;
    int slot;
};


int tcpconnection_accept(int epollfd, int listenfd);
void connection_broadcast(const char *buff, int len);
int connection_close(struct connection *conn);

#endif
