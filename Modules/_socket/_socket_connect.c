#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/un.h>
#include <arpa/inet.h>

#include "../Includes/_socket.h"
#include "../Includes/_fs.h"
#include "../Includes/_modules.h"
#include "../../parser/Includes/bytecode.h"
#include "../../parser/Includes/bobject.h"


Bobject *close_Socket_socket_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    if (close(SOCKID->sockid) == -1)
        return _sys_error(bstate, errno);

    return bstate->none;
}

Bobject *listen_Socket_socket_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    if (args){
        if (listen(SOCKID->sockid, BARG[0]->value.num_value) == -1)
            return _sys_error(bstate, errno);
    }
    return bstate->none;
}

Bobject *shutdown_Socket_socket_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    int how = 0;
    if (args)
        how = (int)(BARG[0]->value.num_value);

    if (shutdown(SOCKID->sockid, how))
        return _sys_error(bstate, errno);

    return bstate->none;
}

int mk_hostname(Bobject *args, struct sockaddr_in* address, bcon_State* bstate){
    Bobject** arg = BARG;

    if (args->value.num_value < 1){
        address->sin_addr.s_addr = INADDR_ANY;
        address->sin_port = htons(arg[0]->value.num_value);

    } else {
        address->sin_port = htons(arg[1]->value.num_value);
        
        struct hostent *hostinfo;
        hostinfo = gethostbyname(arg[0]->value.str_value);

        if (hostinfo == NULL){
            perror("Something went wrong");
            return -1;
        }
        
        address->sin_addr = *(struct in_addr *)hostinfo->h_addr;
    }
    return 0;
}

Bobject *bind_Socket_socket_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    if (!args || args->value.num_value < 2)
        return _type_error(bstate, "Socket.bind takes 2 args (*hostNameString, portNumber)");

    int sockid = SOCKID->sockid;
    struct sockaddr_in* address = SOCKID->address;

    if (mk_hostname(args, address, bstate))
        return _sys_error(bstate, errno);

    if (bind(sockid, (struct sockaddr *)address, sizeof(struct sockaddr_in)) < 0)
        return _sys_error(bstate, errno);

    return bstate->none;
}

Bobject *connect_Socket_socket_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    if (!args || args->value.num_value < 2)
        return _type_error(bstate, "Socket.connect takes 2 args (*hostNameString, portNumber)");

    int sockid = SOCKID->sockid;
    struct sockaddr_in* address = SOCKID->address;

    if (mk_hostname(args, address, bstate))
        return _sys_error(bstate, errno);

    if (connect(sockid, (struct sockaddr *)address, sizeof(struct sockaddr_in)) < 0)
        return _sys_error(bstate, errno);

    return bstate->none;
}

Bobject *send_Socket_socket_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    if (!args || args->value.num_value < 1)
        return _type_error(bstate, "Socket.send takes atleast 1 arg (Bytes)");

    int flag = 0;
    Bobject **arg = BARG;

    if (args->value.num_value > 1)
        flag = (int)(arg[1]->value.num_value);

    char* msg = arg[0]->value.str_value;

    if (send(SOCKID->sockid, msg, strlen(msg)+1, flag) == -1)
        return _sys_error(bstate, errno);

    return bstate->none;
}

Bobject *recv_Socket_socket_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    int flag = 0;
    size_t size = 1024;

    Bobject **arg = BARG;

    if (args) {
        size = (size_t)(arg[0]->value.num_value);
        size = (size < 1) ? 1024: size;
        
        if (args->value.num_value > 1)
            flag = (int)(arg[1]->value.num_value);
    }

    char* buffer = malloc(size);
    int data = recv(SOCKID->sockid, buffer, size, flag);

    if (data < 0 || buffer == NULL)
        return _sys_error(bstate, errno);

    if (data == 0) {
        free(buffer);
        return bstate->none;
    }

    buffer = realloc(buffer, data);
    return mk_string(buffer, bstate);
}

Bobject *accept_Socket_socket_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    bsocket* bsock = SOCKID;

    int sockid = bsock->sockid;
    struct sockaddr_in* address = bsock->address;

    size_t addrlen = sizeof(struct sockaddr_in);
    int client = accept(sockid, (struct sockaddr *)address, (socklen_t*)&addrlen);

    if (client == -1)
        return _sys_error(bstate, errno);

    bsocket *_newSocket = malloc(sizeof(bsocket));
    _newSocket->sockid = client;

    Stack *M = create_stack(9);
    add_to_stack_i(M, "close", mk_module_fun("[Method Socket.close]", close_Socket_socket_bn, (void *)_newSocket));
    add_to_stack_i(M, "recv", mk_module_fun("[Method Socket.recv]", recv_Socket_socket_bn, (void *)_newSocket));
    add_to_stack_i(M, "send", mk_module_fun("[Method Socket.send]", send_Socket_socket_bn, (void *)_newSocket));

    bclass *mod = malloc(sizeof(bclass));
    mod->attrs = M;

    Bobject *bin = malloc(sizeof(Bobject));
    bin->value.str_value = "[ClientObject Socket]";
    bin->type = BOBJECT;
    bin->d = mod;

    return bin;
}

Bobject *Socket_socket_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    if (!args || args->value.num_value < 2)
        return _type_error(bstate, "'Socket.Socket()' expects atmost 2 number arguments (addrFamily, sockType, Protocol)");

    int Protocol = 0;
    Bobject** arg = BARG;

    if (args->value.num_value > 2 && arg[2]->type == BNUMBER)
        Protocol = (int)(arg[2]->value.num_value);

    int AF = (int)(arg[0]->value.num_value);
    int SOCK = (int)(arg[1]->value.num_value);

    int sockid = socket(AF, SOCK, Protocol);

    if (sockid == -1){
        bstate->islocked = BLOCK_ERRORED;
        return _sys_error(bstate, errno);
    }

    struct sockaddr_in* address = malloc(sizeof(struct sockaddr_in));
    address->sin_family = AF;
    address->sin_addr.s_addr = INADDR_ANY;

    bsocket* _Socket = malloc(sizeof(bsocket));
    _Socket->sockid = sockid;
    _Socket->address = address;

    Stack *M = create_stack(9);
    add_to_stack_i(M, "accept", mk_module_fun("[Method Socket.accept]", accept_Socket_socket_bn, (void *)_Socket));
    add_to_stack_i(M, "bind", mk_module_fun("[Method Socket.bind]", bind_Socket_socket_bn, (void *)_Socket));
    add_to_stack_i(M, "close", mk_module_fun("[Method Socket.close]", close_Socket_socket_bn, (void *)_Socket));
    add_to_stack_i(M, "connect", mk_module_fun("[Method Socket.connect]", connect_Socket_socket_bn, (void *)_Socket));
    add_to_stack_i(M, "listen", mk_module_fun("[Method Socket.listen]", listen_Socket_socket_bn, (void *)_Socket));
    add_to_stack_i(M, "recv", mk_module_fun("[Method Socket.recv]", recv_Socket_socket_bn, (void *)_Socket));
    add_to_stack_i(M, "send", mk_module_fun("[Method Socket.send]", send_Socket_socket_bn, (void *)_Socket));
    add_to_stack_i(M, "shutdown", mk_module_fun("[Method Socket.shutdown]", shutdown_Socket_socket_bn, (void *)_Socket));

    bclass *mod = malloc(sizeof(bclass));
    mod->attrs = M;

    Bobject *bin = malloc(sizeof(Bobject));
    bin->value.str_value = "[ServerObject Socket]";
    bin->type = BOBJECT;
    bin->d = mod;

    return bin;
}

