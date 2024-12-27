#ifndef H_BYTEC
#define H_BYTEC

#include "bcode.h"
#include "bobject.h"

typedef struct BYTEC_UP{
	Bcode* tok;
} Bytec;

// control interpreter state
#define BLOCK_BROKE 1
#define BLOCK_RETURNED 0
#define BLOCK_CONTINUE 3
#define BLOCK_ISRUNNING 2
// end


Bobject *mk_safe_Bobject();

Bobject *bt_make_variable(Bcode *code, bcon_State *bstate);

Bobject *bt_num(Bcode *code, bcon_State *bstate);

Bobject *bt_add_num(Bcode *code, bcon_State *bstate);

Bobject *bt_minus_num(Bcode *code, bcon_State *bstate);

Bobject *bt_div_num(Bcode *code, bcon_State *bstate);

Bobject *bt_mult_num(Bcode *code, bcon_State *bstate);

Bobject *bt_not(Bcode *code, bcon_State *bstate);

Bobject *bt_equals(Bcode *code, bcon_State *bstate);

Bobject *bt_not_equals(Bcode *code, bcon_State *bstate);

Bobject *bt_less_than_num(Bcode *code, bcon_State *bstate);

Bobject *bt_greater_than_num(Bcode *code, bcon_State *bstate);

Bobject *bt_less_equals_num(Bcode *code, bcon_State *bstate);

Bobject *bt_greater_equals_num(Bcode *code, bcon_State *bstate);

Bobject *bt_str(Bcode *code, bcon_State *bstate);

Bobject *bt_list(Bcode *code, bcon_State *bstate);

Bobject *bt_get_variable(Bcode *code, bcon_State *bstate);

Bobject *bt_if(Bcode *code, bcon_State *bstate);

Bobject *bt_while(Bcode *code, bcon_State *bstate);

Bobject *bt_break(Bcode *code, bcon_State *bstate);

Bobject *bt_continue(Bcode *code, bcon_State *bstate);

Bobject *bt_get_attribute(Bcode *code, bcon_State *bstate);

Bobject *bt_set_attribute(Bcode *code, bcon_State *bstate);

Bobject *bt_mk_fun(Bcode *code, bcon_State *bstate);

Bobject *bt_call_function(Bcode *code, bcon_State *bstate);

Bobject *bt_make_b_fun(Bobject *(*fun)(Bobject *, Bobject *, bcon_State *));

Bobject *bt_mk_class(Bcode *code, bcon_State *bstate);

Stack *mk_type(bcon_State *bstate, Bcode *code, char *name);

Bobject *bt_mk_string(char *name);

Bobject *bt_exec_type(Bobject *args, Bobject *fun, bcon_State *bstate);

Stack *mk_object(Bobject *cls, bcon_State *bstate, Bcode *code, char *name);

void save_object_block(Bobject *cls, Bcode *code, bcon_State *state, Stack *attr);

void save_type_block(Bcode *code, bcon_State *state, Stack *attr);

void block_evaluator_start(Bcode *run, bcon_State *state);

void append_param(Param **start, Param **recent, Param *new_token);

Bobject *object_copy(Bobject *copy);

Param *mk_param(Bcode *code);

Bobject *mk_def_param(Bcode *arg, bcon_State *state);

Bobject *bt_return(Bcode *arg, bcon_State *state);

Bobject *bt_mk_fun(Bcode *code, bcon_State *state);

Bobject *bt_call_function(Bcode *code, bcon_State *state);

Bobject *bt_exec_function(Bobject *args, Bobject *fun, bcon_State *state);

// making function arguments
void append_fun_args_data(Bobject **start, Bobject **recent, Bobject *new_token);

Bytec *bytec_read(Bcode *toks);

Bobject *bt_make_b_fun(Bobject *(*fun)(Bobject *, Bobject *, bcon_State *));

Bobject *bt_mk_class(Bcode *code, bcon_State *bstate);

Stack *mk_type(bcon_State *bstate, Bcode *code, char *name);

// makes the type functions from bytecode
void save_type_block(Bcode *code, bcon_State *bstate, Stack *attr);

// makes the object functions from bytecodes
// appends the self to the class function
void save_object_block(Bobject *cls, Bcode *code, bcon_State *bstate, Stack *attr);

Stack *mk_object(Bobject *cls, bcon_State *bstate, Bcode *code, char *name);

Bobject *bt_exec_type(Bobject *args, Bobject *fun, bcon_State *bstate);

#endif
