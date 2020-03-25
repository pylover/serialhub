#include "common.h"
#include "settings.h"

#include <arpa/inet.h>


int tcp_bindandlisten(struct sockaddr_in *listenaddr) {
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

