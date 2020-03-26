#include "common.h"
#include "circularbuffer.h"
#include "connection.h"
#include "settings.h"

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>   


#define CONNECTIONEVENTS   ( EPOLLIN | EPOLLRDHUP )


static struct connection* connections[MAXCONNECTIONS];


static int _getfreeslot() {
    int i;
    for (i = 0; i < MAXCONNECTIONS; i++) {
        if (connections[i] == NULL) {
            return i;
        }
    }
    return -1;
}


int connection_registerevents(struct connection *conn) {
    int err;
	struct epoll_event ev;

	ev.events = CONNECTIONEVENTS;
	ev.data.ptr = conn;
	err = epoll_ctl(epollfd, EPOLL_CTL_ADD, conn->sockfd, &ev);
	if (err == ERR) {
		L_ERROR("epoll_ctl: sockfd");
        return ERR;
	}
	return OK;
}


int connection_unregisterevents(struct connection *conn) {
    int err;
	struct epoll_event ev;

	ev.events = CONNECTIONEVENTS;
	ev.data.ptr = conn;
	err = epoll_ctl(epollfd, EPOLL_CTL_DEL, conn->sockfd, &ev);
	if (err == ERR) {
		L_ERROR("epoll_ctl: sockfd");
        return ERR;
	}
	return OK;
}



void connection_broadcast(const char *buff, int len) {
    int i, err;
    for (i = 0; i < MAXCONNECTIONS; i++) {
        if (connections[i] == NULL) {
            break;
        }
        err = write(connections[i]->sockfd, buff, len);
        if (err <= 0) {
            connection_close(connections[i]);
        }
    }
}


int tcpconnection_accept(int epollfd, int listenfd) {
	int err, sockfd, slot;
	struct sockaddr addr; 
    struct connection *conn;
	socklen_t addrlen = sizeof(struct sockaddr);
    
	sockfd = accept(listenfd, &addr, &addrlen);
	if (sockfd == ERR) {
		L_ERROR("tcp accept");
        return ERR;
	}

    slot = _getfreeslot();
    if (slot == ERR) {
        // No free spot, rejecting
        L_INFO("Rejecting connection");
        err = close(sockfd);
        if (err == ERR) {
            L_ERROR("Cannot close connection");
            return err;
        }
        return OK;
    }
    
    conn = malloc(sizeof(struct connection));
    if (conn == NULL) {
        L_ERROR("Cannot allocate connection memory");
        return ERR;
    }
    
    struct sockaddr_in *ii = (struct sockaddr_in*)&addr;
    L_INFO("New connection: %s:%d", inet_ntoa(ii->sin_addr), ii->sin_port);
    conn->slot = slot;
    conn->sockfd = sockfd;
    conn->type = CNTYPE_TCP;
    conn->address = (struct sockaddr_in*) &addr;
    connections[slot] = conn;
    return connection_registerevents(conn);
}


int connection_close(struct connection *conn) {
    int err;
    
    err = connection_unregisterevents(conn);
    if (err == ERR) {
        L_ERROR("Cannot delete connection epoll events");
        return err;
    }

    err = close(conn->sockfd);
    if (err == ERR) {
        L_ERROR("Cannot close connection");
        return err;
    }
   
    L_INFO("removing connection: %d", conn->slot);
    connections[conn->slot] = NULL;
    free(conn);
    return OK;
}


int tcpconnection_listen(struct sockaddr_in *listenaddr) {
    int listenfd;
	int option = 1;
	int err;

	listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

	memset(listenaddr, '0', sizeof(&listenaddr));
	listenaddr->sin_family = AF_INET;
	
	if (settings.tcpbind == NULL) {
		listenaddr->sin_addr.s_addr = htonl(INADDR_ANY);
	} 
    else if(inet_pton(AF_INET, settings.tcpbind, &listenaddr->sin_addr)<=0) {
        L_ERROR("Invalid address: %s", settings.tcpbind);
        return ERR;
	}
	listenaddr->sin_port = htons(settings.tcpport); 
	err = bind(listenfd, (struct sockaddr*)listenaddr, sizeof(*listenaddr)); 
	if (err) {
		L_ERROR("Cannot bind on: %s", inet_ntoa(listenaddr->sin_addr));
        return ERR;
	}
	
	err = listen(listenfd, settings.tcpbacklog); 
	if (err) {
		L_ERROR("Cannot listen on: %s", inet_ntoa(listenaddr->sin_addr));
        return ERR;
	}
	L_INFO(
		"Listening on %s:%d", 
		inet_ntoa(listenaddr->sin_addr),
		ntohs(listenaddr->sin_port)
	);
    return listenfd;
}

