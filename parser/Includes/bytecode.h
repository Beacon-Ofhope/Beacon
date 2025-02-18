#ifndef H_BYTEC
#define H_BYTEC

#include "bcode.h"
#include "bobject.h"

typedef struct BYTEC_UP{
	Bcode* tok;
} Bytec;

// control interpreter state
#define BLOCK_RETURNED 0
#define BLOCK_BROKE 1
#define BLOCK_ISRUNNING 2
#define BLOCK_CONTINUE 3
#define BLOCK_ERRORED 4
// end


Bobject *mk_safe_Bobject();

char *check_type(Bobject *data);

Bobject *bt_make_variable(Bcode *code, bcon_State *bstate);

Bobject *num_malloc(Bobject *a, Bobject *b, double ans);

Bobject *data_malloc(Bobject *a, Bobject *b, int ans);

Bobject *bt_num(Bcode *code, bcon_State *bstate);

Bobject *bt_add_num(Bcode *code, bcon_State *bstate);

Bobject *bt_minus_num(Bcode *code, bcon_State *bstate);

Bobject *bt_div_num(Bcode *code, bcon_State *bstate);

Bobject *bt_mult_num(Bcode *code, bcon_State *bstate);

Bobject *bt_mod_num(Bcode *code, bcon_State *bstate);

Bobject *bt_not(Bcode *code, bcon_State *bstate);

Bobject *bt_equals(Bcode *code, bcon_State *bstate);

Bobject *bt_not_equals(Bcode *code, bcon_State *bstate);

Bobject *bt_less_than_num(Bcode *code, bcon_State *bstate);

Bobject *bt_greater_than_num(Bcode *code, bcon_State *bstate);

Bobject *bt_less_equals_num(Bcode *code, bcon_State *bstate);

Bobject *bt_greater_equals_num(Bcode *code, bcon_State *bstate);

Bobject *bt_str(Bcode *code, bcon_State *bstate);

Bobject *bt_none(Bcode *code, bcon_State *bstate);

Bobject *bt_face(Bcode *code, bcon_State *bstate);

Bobject *bt_get_variable(Bcode *code, bcon_State *bstate);

Bobject *bt_if(Bcode *code, bcon_State *bstate);

Bobject *bt_try(Bcode *code, bcon_State *bstate);

Bobject *bt_while(Bcode *code, bcon_State *bstate);

Bobject *bt_break(Bcode *code, bcon_State *bstate);

Bobject *bt_continue(Bcode *code, bcon_State *bstate);

Bobject *bt_get_attribute(Bcode *code, bcon_State *bstate);

Bobject *bt_set_attribute(Bcode *code, bcon_State *bstate);

Bobject *bt_mk_fun(Bcode *code, bcon_State *bstate);

Bobject *bt_call_function(Bcode *code, bcon_State *bstate);

Bobject *bt_mk_class(Bcode *code, bcon_State *bstate);

Stack *mk_type(bcon_State *bstate, char *name);

void block_evaluator_start(Bcode *run, bcon_State *state);

void append_param(Param **start, Param **recent, Param *new_token);

Bobject *bt_return(Bcode *arg, bcon_State *state);

Bobject *bt_throw(Bcode *code, bcon_State *bstate);

Bobject *bt_import(Bcode *code, bcon_State *bstate);

Bobject *bt_mk_fun(Bcode *code, bcon_State *state);

Bobject *bt_call_function(Bcode *code, bcon_State *state);

Bobject *bt_exec_type(bargs *args, btype *fun, bcon_State *bstate);

// Bobject *bt_exec_function(bargs *args, Bobject *fun, bcon_State *state);
Bobject *bt_exec_function(bargs *args, bfunction *fn, bcon_State *bstate);

Bobject *bt_mk_class(Bcode *code, bcon_State *bstate);

// makes the type functions from bytecode
Bobject *mk_object(Bobject *obj, btype *fun);

Bobject *bt_exec_type(bargs *args, btype* fun, bcon_State * bstate);

// DICTONARY
Bobject *bt_dict(Bcode *code, bcon_State *bstate);


// LIST
Bobject *bt_list(Bcode *code, bcon_State *bstate);

Bobject *pop_list(blist *List, Bobject *index, bcon_State *bstate);

Bobject *copy_list(blist *List, bcon_State *bstate);

int append_to_list(blist* List, Bobject* value);

Bobject* insert_in_list(blist* List, Bobject* value, size_t index, bcon_State* bstate);

// ERRORS
Bobject *_attribute_error(bcon_State *bstate, Bobject *data, unsigned int line);

Bobject *_no_attribute_error(bcon_State *bstate, Bobject *data, char *attr_name, unsigned int line);

Bobject *_set_attribute_error(bcon_State *bstate, Bobject *data, unsigned int line);

Bobject *_reference_error(bcon_State *bstate, char *name, unsigned int line);

Bobject *_sys_error(bcon_State *bstate, int error_no);

Bobject* _type_error(bcon_State *bstate, char *message, unsigned int line);

Bobject *_runtime_error(bcon_State *bstate, char *message);

Bobject *_import_error(bcon_State *bstate, char *message);

Bobject* _import_error2(bcon_State *bstate, char *message, unsigned int line);

#endif
