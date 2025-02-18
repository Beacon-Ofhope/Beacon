#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

#include "Includes/_list.h"
#include "Includes/_imports.h"
#include "Includes/_modules.h"
#include "../parser/Includes/bobject.h"
#include "../parser/Includes/bytecode.h"


Bobject *append_list_bn(bargs* args, bfunction2 *fun, bcon_State *bstate) {
	if (!args || args->count < 2 )
		return _type_error(bstate, "'List.append' takes 2 arguments (List, Object).", args->line);

	Bobject** arg = BARG;

    if (append_to_list(arg[0]->value.list, arg[1]))
        return _sys_error(bstate, errno);

	return bstate->none;
}

Bobject *pop_list_bn(bargs* args, bfunction2 *fun, bcon_State *bstate) {
	if (!args)
		return _type_error(bstate, "'List.append' takes 2 arguments (List, index_number).", args->line);

	Bobject** arg = BARG;
    Bobject* index = NULL;

    if (args->count > 1)
        index = arg[1];

    return pop_list(arg[0]->value.list, index, bstate);
}

Bobject *insert_list_bn(bargs* args, bfunction2 *fun, bcon_State *bstate) {
	if (!args || args->count != 3)
		return _type_error(bstate, "'List.insert' takes 3 arguments (List, index_number, object).", args->line);

	Bobject** arg = BARG;
    return insert_in_list(arg[0]->value.list, arg[2], (size_t)(arg[1]->value.num_value), bstate);
}

Bobject *copy_list_bn(bargs* args, bfunction2 *fun, bcon_State *bstate) {

	Bobject** arg = BARG;
    return copy_list(arg[0]->value.list, bstate);
}

Bobject *Bn_List() {
    Stack *M = create_stack(6);

	add_to_stack(M, "append", mk_module_fun("append", append_list_bn, NULL));
	add_to_stack(M, "copy", mk_module_fun("copy", copy_list_bn, NULL));
	add_to_stack(M, "pop", mk_module_fun("pop", pop_list_bn, NULL));
	add_to_stack(M, "insert", mk_module_fun("insert", insert_list_bn, NULL));

	bmodule *mod = malloc(sizeof(bmodule));
	mod->name = "List";
	mod->attrs = M;

	Bobject *bin = malloc(sizeof(Bobject));
	bin->value.module = mod;
	bin->type = BMODULE;
	bin->refs = 0;

	return bin;
}
