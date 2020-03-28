#include "common.h"
#include "mux.h"

#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>


int tcpconnection_listen() {
    int listenfd;
	int option = 1;
	int err;
    struct epoll_event ev;
    struct sockaddr_in listenaddr;

	listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    
    memset(&listenaddr, 0, sizeof(struct sockaddr_in));
	listenaddr.sin_family = AF_INET;
	
	if (settings.tcpbind == NULL) {
		listenaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	} 
    else if(inet_pton(AF_INET, settings.tcpbind, &listenaddr.sin_addr)<=0) {
        perrorf("Invalid address: %s", settings.tcpbind);
        return ERR;
	}
	listenaddr.sin_port = htons(settings.tcpport); 
	err = bind(listenfd, (struct sockaddr*)&listenaddr, sizeof(listenaddr)); 
	if (err) {
		perrorf("Cannot bind on: %s", inet_ntoa(listenaddr.sin_addr));
        return ERR;
	}
	
	err = listen(listenfd, settings.tcpbacklog); 
	if (err) {
		perrorf("Cannot listen on: %s", inet_ntoa(listenaddr.sin_addr));
        return ERR;
	}
    printsocket("Listening on: ", listenaddr);

    // epoll events
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = listenfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) == ERR) {
        perrorf("epoll_ctl: EPOLL_CTL_ADD, tcplisten socket");
        exit(EXIT_FAILURE);
    }

    return listenfd;
}


int tcpconnection_accept(int listenfd) {
	int err, sockfd;
	struct sockaddr addr; 
	socklen_t addrlen = sizeof(struct sockaddr);
    
	sockfd = accept(listenfd, &addr, &addrlen);
	if (sockfd == ERR) {
        if (errno == EBADF) {
            return ERR;
        }
		perrorf("Cannot accept new tcp connection");
        // Ignoring error, see accept(2)
        return OK;
	}
    
    struct sockaddr_in *ii = (struct sockaddr_in*)&addr;
    printfln(
        "New TCP connection: %s:%d", 
        inet_ntoa(ii->sin_addr), 
        ii->sin_port
    );
    
    err = connection_add(sockfd, addr, CNTYPE_TCP);
    if (err == ERR) {
		perrorf("Cannot accept new tcp connection");
        // Just cannot handle the new connection.
    }
    return OK;
}

