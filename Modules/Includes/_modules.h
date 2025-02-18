#ifndef H_MODULES_LIB
#define H_MODULES_LIB

#include "../../parser/Includes/bobject.h"

#define BARG (args->argv)

Bobject *mk_number(double number);

Bobject *mk_string(char *str);

Bobject *mk_module_fun(char *name, Bobject *(*fun)(bargs *, bfunction2 *, bcon_State *), void *ptr);

Bobject *mk_mod_number(double x);

Bobject *mk_mod_string(char *s);

Bobject *mk_mod_interface(char *name, Stack *attrs, unsigned char count, bcon_State *bstate);

#endif