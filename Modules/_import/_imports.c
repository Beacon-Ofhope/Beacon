#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "../Includes/_imports.h"
#include "../Includes/_modules.h"
#include "../../parser/Includes/bobject.h"
#include "../../parser/Includes/bytecode.h"

#include "../../Include/brepl.h"


Bobject *import_module_then(char *name, bcon_State *bstate){
	struct stat *buffer = malloc(sizeof(struct stat));
	stat(name, buffer);

	if (S_ISREG(buffer->st_mode)){
		free(buffer);
		return import_file_then(name, bstate);

	} else if (S_ISDIR(buffer->st_mode)){
		free(buffer);
		char *file = my_concat(name, "/app.bc", 0);

		if (access(file, F_OK) == 0)
			return import_file_then(file, bstate);

		free(file);
	}
	return NULL;
}

char* mk_import_path(char *name, bcon_State* bstate) {
	switch (name[0]){
		case '.':
			return mk_path_name(name, mk_base_folder(bstate->file));
		case '/':
			return mk_path_name_here(name);
		default:
			break;
	}
	return mk_path_name_from_beacon_modules(name, bstate->base_folder);
}

Bobject* file_imports_bn(char *name, bcon_State *bstate){
	if (strlen(name)) {
		char *module_name = mk_import_path(name, bstate);
		
		if (module_name)
			return import_module_then(module_name, bstate);
	}
	
	return NULL;
}
