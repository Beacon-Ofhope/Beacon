#include <stdlib.h>
#include <string.h>

#include "Includes/_random.h"
#include "Includes/_imports.h"
#include "Includes/_modules.h"
#include "../parser/Includes/bobject.h"
#include "../parser/Includes/bytecode.h"


Bobject *random_random_bn(Bobject* args, Bobject* fun, bcon_State *bstate){
	double rd;

	return mk_mod_number(rd);
}


Bobject *Bn_Random() {
    Stack *M = create_stack(4);
    
    add_to_stack(M, "random", mk_module_fun("[Function Random.random]", random_random_bn, NULL));

	bmodule *mod = malloc(sizeof(bmodule));
	mod->attrs = M;

	Bobject *bin = mk_safe_Bobject();
	bin->value.str_value = "[Module Random]";
	bin->type = BMODULE;
	bin->d = mod;

	return bin;
}

