#ifndef H_BEACON_IMPORTS
#define H_BEACON_IMPORTS

#include "../../parser/Includes/bobject.h"

Bobject *file_imports_bn(char *name, bcon_State *bstate);

char *mk_relative_path(char* name);

char *mk_path_name(char *name, char *base);

char* my_concat(char* a, const char* b, int wipe);

char *mk_path_name_here(char *name);

char *mk_path_name_from_beacon_modules(char *name, char *base);

char* mk_module_name(char* name);

Bobject *import_file_then(char *name, bcon_State *bstate);

#endif