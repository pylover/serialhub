#ifndef H_CONNECTION
#define H_CONNECTION

#include <sys/socket.h>


enum connectiontype {  
    CNTYPE_TCP,
    CNTYPE_UNIX,
}; 


struct connection{
    int sockfd;
    enum connectiontype type;
    struct sockaddr address;
    int slot;
};


void connection_broadcast(const char *buff, int len);
int connection_close(struct connection *conn);

int tcpconnection_listen();
int tcpconnection_accept(int listenfd);

int unixconnection_listen();
int unixconnection_accept(int listenfd);
#endif
