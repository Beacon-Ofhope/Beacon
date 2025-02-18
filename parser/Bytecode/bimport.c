#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <bits/types/siginfo_t.h>

#include "../Includes/parser.h"
#include "../Includes/bcode.h"
#include "../Includes/bobject.h"
#include "../Includes/bytecode.h"

#include "../../Modules/Includes/_imports.h"

void import_queued_module(pport *mod, bcon_State *bstate, unsigned short line){
	Bobject *module = get_from_stack(bstate->modules, mod->name);

	if (module == NULL){
		module = file_imports_bn(mod->name, bstate);

		if (module == NULL){
			_import_error(bstate, mod->name);
			return;
		}
	}

	if (bstate->islocked == BLOCK_ISRUNNING)
		add_to_stack(bstate->memory, mod->alias, module);
}

void import_queue(Bcode *code, bcon_State *bstate){
	pimport *imp = code->value.pimport;
	Bobject *module;

	if (imp->count == 0)
		import_queued_module(imp->imports.dir, bstate, code->line);
	
	else {
		int count = imp->count;
		pport **mods = imp->imports.dirs;
		
		for(int i = 0; i < count; ++i) {
			import_queued_module(mods[i], bstate, code->line);

			if (bstate->islocked == BLOCK_ERRORED)
				break;
		}
	}
}

Bobject *import_from_module(char *path, bcon_State *bstate, unsigned short line){
	Bobject *module = get_from_stack(bstate->modules, path);

	if (module == NULL){
		module = file_imports_bn(path, bstate);

		if (module == NULL)
			return _import_error(bstate, path);
	}
	return module;
}

void import_from_path(Bcode *code, bcon_State *bstate){
	pimport *imp = code->value.pimport;
	Bobject *module = import_from_module(imp->path, bstate, code->line);

	if (bstate->islocked == BLOCK_ERRORED)
		return;

	pport *mod;
	Bobject *attr;

	if (imp->count == 0){
		mod = imp->imports.dir;
		attr = get_from_stack(module->value.module->attrs, mod->name);

		if (attr == NULL){
			_import_error2(bstate, "can't import name '' from module ''", code->line);
			return;
		}

		add_to_stack(bstate->memory, mod->alias, attr);
		
	} else {
		int count = imp->count;
		pport **mods = imp->imports.dirs;

		for (int i = 0; i < count; ++i){
			mod = mods[i];
			attr = get_from_stack(module->value.module->attrs, mod->name);

			if (attr == NULL){
				_import_error2(bstate, "can't import name '' from module ''", code->line);
				return;
			}

			add_to_stack(bstate->memory, mod->alias, attr);
		}
	}
}

Bobject *bt_import(Bcode *code, bcon_State *bstate){
	if (code->value.pimport->path == NULL)
		import_queue(code, bstate);
	else 
		import_from_path(code, bstate);

	return bstate->none;
}
