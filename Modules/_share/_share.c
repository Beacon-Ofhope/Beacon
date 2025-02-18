#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Includes/_share.h"
#include "../Includes/_imports.h"
#include "../Includes/_modules.h"
#include "../../parser/Includes/bobject.h"
#include "../../parser/Includes/bytecode.h"


Bobject *require_bn(bargs *args, bfunction2 *fun, bcon_State *bstate) {
	if (!args)
		return _type_error(bstate, "'console.require()' takes 1 argument (module_name).", args->line);

	Bobject* arg = BARG[0];
	Bobject *module = get_from_stack(bstate->modules, arg->value.str_value);

	if (module == NULL){
		module = file_imports_bn(arg->value.str_value, bstate);

		if (module == NULL)
			return _import_error(bstate, arg->value.str_value);
	}

	return module;
}

Bobject *print_bn(bargs* args, bfunction2 *fun, bcon_State *bstate){
	if (args) {
		Bobject** arg = BARG;

		int count = args->count;
		int count_stop = count-1;

		for(int i = 0; i < count; i++){
			print_data_share(arg[i]);

			if (i == count_stop)
				break;
			
			putchar(' ');
		}
		putchar('\n');
	}
	return bstate->none;
}

Bobject* input_bn(bargs* args, bfunction2 *fun, bcon_State *bstate){
	if (args)
		fputs(BARG[0]->value.str_value, stdout);

	size_t n = 0;
	char *buffer = NULL;

	size_t size = getline(&buffer, &n, stdin);

	if (size == -1 || size == 0)
		return mk_string("\0");

	buffer = realloc(buffer, size);
	buffer[size-1] = '\0';

	return mk_string(buffer);
}

Bobject* quit_share_bn(bargs* args, bfunction2 *fun, bcon_State *bstate){
	if (args == NULL || args->argv[0]->type != BNUMBER)
		return _type_error(bstate, "share.quit expects a (number) as exit status code.", args->line);

	exit(((int)(args->argv[0]->value.num_value)));
	return bstate->none;
}

Bobject* vars_share_bn(bargs* args, bfunction2 *fun, bcon_State *bstate){
	Bobject *dt = mk_Bobject(bstate);
	dt->type = BDICT;

	if (args) {
		Bobject *arg = args->argv[0];

		switch (arg->type){
			case BFUNCTION:
				dt->value.attrs = arg->value.bfun->bstate->memory;
				break;
			case BTYPE:
				dt->value.attrs = arg->value.btype->bstate->memory;
				break;
			default:
				free(dt);
				return _type_error(bstate, "share.vars expects atmost 1 argument of type (function or type).", args->line);
				break;
		}
	} else {
		dt->value.attrs = bstate->memory;
	}

	return dt;
}

Bobject* type_share_bn(bargs* args, bfunction2 *fun, bcon_State *bstate){
	if (!args) 
		return _type_error(bstate, "share.type expects 1 argument (any)", args->line);

	char *_type;

	switch (args->argv[0]->type) {
		case BSTRING:
			_type = "string";
			break;
		case BNUMBER:
			_type = "number";
			break;
		case BLIST:
			_type = "list";
			break;
		case BDICT:
			_type = "dictionary";
			break;
		case BMODULE:
			_type = "module";
			break;
		case BFUNCTION2:
		case BFUNCTION:
			_type = "function";
			break;
		case BINTERFACE:
			_type = "interface";
			break;
		case BTYPE:
			_type = args->argv[0]->value.btype->name;
			break;
		case BOBJECT:
			_type = args->argv[0]->value.bclass->name;
			break;
		default:
			_type = "none";
			break;
	}

	return mk_string(strdup(_type));
}

Bobject* binary_share_bn(bargs* args, bfunction2 *fun, bcon_State *bstate){
	int b = (int)(args->argv[0]->value.num_value);
	

	return mk_string("bin");
}

Bobject* dir_share_bn(bargs* args, bfunction2 *fun, bcon_State *bstate){

	return bstate->none;
}

Bobject *Bn_Share() {
    Stack *M = create_stack(20);

	add_to_stack(M, "bin", mk_module_fun("bin", binary_share_bn, NULL));
	add_to_stack(M, "False", mk_number(0));
	add_to_stack(M, "dir", mk_module_fun("dir", dir_share_bn, NULL));
	add_to_stack(M, "input", mk_module_fun("input", input_bn, NULL));
	add_to_stack(M, "print", mk_module_fun("print", print_bn, NULL));
	add_to_stack(M, "quit", mk_module_fun("quit", quit_share_bn, NULL));
	add_to_stack(M, "require", mk_module_fun("require", require_bn, NULL));
	add_to_stack(M, "True", mk_number(1));
	add_to_stack(M, "type", mk_module_fun("type", type_share_bn, NULL));
	add_to_stack(M, "vars", mk_module_fun("vars", vars_share_bn, NULL));

	bmodule *mod = malloc(sizeof(bmodule));
	mod->name = "Share";
	mod->attrs = M;

	Bobject *bin = malloc(sizeof(Bobject));
	bin->value.module = mod;
	bin->type = BMODULE;
	bin->refs = 0;

	return bin;
}

