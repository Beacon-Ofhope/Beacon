#ifndef H_SOCKET_LIB
#define H_SOCKET_LIB

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include "../../parser/Includes/bobject.h"

typedef struct BSOCKET {
    int sockid;
    struct sockaddr_in* address;
} bsocket;

#define SOCKID ((bsocket *)(((bfunction2 *)(fun->d))->ptr))

Bobject *Socket_socket_bn(Bobject *args, Bobject *fun, bcon_State *bstate);

Bobject *Bn_Socket();

#endif