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
int connection_add(int fd, struct sockaddr addr, enum connectiontype type);
int connection_close(struct connection *conn);

#endif
