
#ifndef H_BYTEC
#define H_BYTEC

#include "dcode.h"
#include "dobject.h"

typedef struct BYTEC_UP{
	Dcode* tok;
} Bytec;

Dobject* bt_make_variable(Dcode* code, Stack* stack);

Dobject* bt_get_variable(Dcode* code, Stack* stack);

Dobject* bt_str(Dcode* code, Stack* stack);

Dobject* bt_num(Dcode* code, Stack* stack);

Dobject* bt_add_num(Dcode* code, Stack* stack);
Dobject* bt_div_num(Dcode* code, Stack* stack);
Dobject* bt_sub_num(Dcode* code, Stack* stack);
Dobject* bt_mult_num(Dcode* code, Stack* stack);
Dobject* bt_modulus_num(Dcode* code, Stack* stack);

Dobject* bt_call_function(Dcode* code, Stack* stack);

Dobject* bt_make_b_function(Dobject* (*fun)(Dobject *, Stack*));

Dobject* bt_make_function(Dcode* code, Stack* stack);


#endif
