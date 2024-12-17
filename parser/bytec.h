#ifndef H_BYTEC
#define H_BYTEC

#include "bcode.h"
#include "bobject.h"

typedef struct BYTEC_UP{
	Bcode* tok;
} Bytec;

Bobject *bt_make_variable(Bcode *code, Stack *stack);

Bobject *bt_num(Bcode *code, Stack *stack);

Bobject *bt_add_num(Bcode *code, Stack *stack);

Bobject *bt_minus_num(Bcode *code, Stack *stack);

Bobject *bt_div_num(Bcode *code, Stack *stack);

Bobject *bt_mult_num(Bcode *code, Stack *stack);

Bobject *bt_str(Bcode *code, Stack *stack);

Bobject *bt_list(Bcode *code, Stack *stack);

Bobject *bt_get_variable(Bcode *code, Stack *stack);

Bobject *bt_if(Bcode *code, Stack *stack);

Bobject *bt_while(Bcode *code, Stack *stack);

Bobject *bt_get_attribute(Bcode *code, Stack *stack);

Bobject *bt_mk_fun(Bcode *code, Stack *stack);

Bobject *bt_call_function(Bcode *code, Stack *stack);

Bobject *bt_make_b_fun(Bobject *(*fun)(Bobject *, Bobject *, Stack *));

Bobject *bt_mk_class(Bcode *code, Stack *stack);

Stack *mk_type(Stack *stack, Bcode *code, char *name);

Bobject *bt_mk_string(char *name);

#endif
