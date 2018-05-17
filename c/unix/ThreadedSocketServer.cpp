#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <strings.h>

#include "ThreadedSocketServer.h"

typedef struct {

    int fd;
    int sock;
    pthread_t thread;

} socket_info_t;

static int accept_connection(int s)
{
    int x;
    struct sockaddr_in sn;

    if(listen(s, 1) == -1) {
        perror("listen()");
        exit(1);
    }

    bzero((char *)&sn, sizeof(sn));
    if((x = accept(s, (struct sockaddr *)NULL, NULL)) == -1) {
        perror("accept()");
        exit(1);
    }
    return x;
}

void * threadfunc(void * arg)
{
    socket_info_t * sockinfo = (socket_info_t *)arg;

    sockinfo->fd = accept_connection(sockinfo->sock);

    return NULL;
}

ThreadedSocketServer::ThreadedSocketServer(int port) 
{
    socket_info_t * sockinfo = new socket_info_t;
    sockinfo->fd = 0;
    _support = (void *)sockinfo;

    struct sockaddr_in sn;
    struct hostent *he;

    if (!(he = gethostbyname("localhost"))) {
        puts("can't gethostname");
        exit(1);
    }

    bzero((char*)&sn, sizeof(sn));
    sn.sin_family = AF_INET;
    sn.sin_port = htons((short)port);
    sn.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((sockinfo->sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket()");
        exit(1);
    }

    if (bind(sockinfo->sock, (struct sockaddr *)&sn, sizeof(sn)) == -1) {
        perror("bind()");
        exit(1);
    }

}

void ThreadedSocketServer::start(void)
{
    socket_info_t * sockinfo = (socket_info_t *)_support;
    if (pthread_create(&sockinfo->thread, NULL, threadfunc, sockinfo) != 0) {
        perror("pthread_create");
    }

}

void ThreadedSocketServer::stop(void)
{
    socket_info_t * sockinfo = (socket_info_t *)_support;
    void * status;
    if (pthread_join(sockinfo->thread, &status) != 0) { 
        perror("pthread_join"); 
    }
}

bool ThreadedSocketServer::connected(void)
{
    socket_info_t * sockinfo = (socket_info_t *)_support;
    return sockinfo->fd > 0;
}

int ThreadedSocketServer::send(char * buf, int len)
{
    socket_info_t * sockinfo = (socket_info_t *)_support;
    return write(sockinfo->fd, buf, len);
}

int ThreadedSocketServer::recv(char * buf, int len)
{
    socket_info_t * sockinfo = (socket_info_t *)_support;
    return read(sockinfo->fd, buf, len);
}