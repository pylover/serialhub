#ifndef H_NETWORKING
#define H_NETWORKING

#include <arpa/inet.h>

int tcp_bindandlisten(struct sockaddr_in *listenaddr);

#endif
