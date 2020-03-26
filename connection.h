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


void connection_broadcast(const char *buff, int len);
int connection_close(struct connection *conn);

int tcpconnection_listen(struct sockaddr_in *listenaddr);
int tcpconnection_accept(int epollfd, int listenfd);

#endif
