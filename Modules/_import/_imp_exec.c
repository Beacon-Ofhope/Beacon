#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>

#include "../Includes/_imports.h"
#include "../Includes/_modules.h"
#include "../../parser/Includes/bobject.h"
#include "../../parser/Includes/bytecode.h"

#include "../../Include/brepl.h"
#include "../../parser/Includes/bcode.h"
#include "../../parser/Includes/eval.h"
#include "../../parser/Includes/lexer.h"
#include "../../parser/Includes/parser.h"


void evaluate_import(char *name, char* code, bcon_State* bstate){
	Lexer *lex = lexer_read(code, name);
	lexer_process(lex);

	Parser *par = parser_read(lex);
	parser_tree(par);
	free(lex);

	Inter *b_interpreter = inter_read(par);
	inter_interpret(b_interpreter);
	free(par);

	Eval *evaluator = evaluator_read(b_interpreter);
	evaluator_start(evaluator, bstate);

	free(b_interpreter);
}

bcon_State* mk_new_bstate(bcon_State* bstate1, char* file){
    bcon_State *bstate = calloc(1, sizeof(bcon_State));

	bstate->file = file;
    bstate->none = bstate1->none;
    bstate->memory = create_stack(0);
    bstate->islocked = BLOCK_ISRUNNING;
	bstate->modules = bstate1->modules;
	bstate->base_folder = bstate1->base_folder;

    add_to_stack(bstate->memory, "console", get_from_stack(bstate1->modules, "Console"));
    return bstate;
}

Bobject* import_file_then(char *name, bcon_State* bstate){
	// using cached
	Bobject* module = get_from_stack(bstate->modules, name);

	if (module)
		return module;

	char *code = open_program_file(name);
	if (code == NULL)
		return NULL;

	bcon_State *module_bstate = mk_new_bstate(bstate, name);

	// cache to support circular imports
	if (true) {
		module = malloc(sizeof(Bobject));
		module->value.str_value = mk_module_name(name);
		module->type = BMODULE;

		bmodule *bin = malloc(sizeof(bmodule));
		bin->attrs = module_bstate->memory;

		module->d = bin;
		add_to_stack(bstate->modules, name, module);
	}

    evaluate_import(name, code, module_bstate);

    if (module_bstate->islocked == BLOCK_ERRORED) {
		module_bstate->islocked = BLOCK_ISRUNNING;

		bstate->islocked = BLOCK_ERRORED;
		bstate->return_value = module_bstate->return_value;

		free(module);
		return bstate->none;
	}

	return module;
}

