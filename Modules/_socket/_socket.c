#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "../Includes/_socket.h"
#include "../Includes/_imports.h"
#include "../Includes/_modules.h"
#include "../../parser/Includes/bobject.h"
#include "../../parser/Includes/bytecode.h"


void add_socketType(Stack *S){
	add_to_stack_i(S, "SOCK_DGRAM", mk_mod_number(SOCK_DGRAM));
	add_to_stack_i(S, "SOCK_RAW", mk_mod_number(SOCK_RAW));
	add_to_stack_i(S, "SOCK_RDM", mk_mod_number(SOCK_RDM));
	add_to_stack_i(S, "SOCK_SEQPACKET", mk_mod_number(SOCK_SEQPACKET));
	add_to_stack_i(S, "SOCK_STREAM", mk_mod_number(SOCK_STREAM));
}

void add_addressFamily(Stack *S){
	add_to_stack_i(S, "AF_APPLETALK", mk_mod_number(AF_APPLETALK));
	add_to_stack_i(S, "AF_INET", mk_mod_number(AF_INET));
	add_to_stack_i(S, "AF_INET6", mk_mod_number(AF_INET6));
	add_to_stack_i(S, "AF_IPX", mk_mod_number(AF_IPX));
	add_to_stack_i(S, "AF_UNIX", mk_mod_number(AF_UNIX));
	add_to_stack_i(S, "AF_UNSPEC", mk_mod_number(AF_UNSPEC));
}

Bobject *Bn_Socket() {
    Stack *M = create_stack(60);
    
    add_socketType(M);
	add_addressFamily(M);

	add_to_stack(M, "Socket", mk_module_fun("[Function: Socket.Socket]", Socket_socket_bn, NULL));

	bmodule *mod = malloc(sizeof(bmodule));
	mod->attrs = M;

	Bobject *bin = mk_safe_Bobject();
	bin->value.str_value = "[Module Console]";
	bin->type = BMODULE;
	bin->d = mod;

	return bin;
}

