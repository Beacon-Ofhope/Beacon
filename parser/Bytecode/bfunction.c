#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/types/siginfo_t.h>

#include "../Includes/bcode.h"
#include "../Includes/bobject.h"
#include "../Includes/bytecode.h"


Param *mk_param(Bcode *code){
    char *name = code->value.str_value;
    Param *param = malloc(sizeof(Param));
    param->next = NULL;
    param->value = name;

    return param;
}

Bobject *mk_def_param(Bcode *arg, bcon_State *bstate){
    Bobject *obj = mk_safe_Bobject();
    obj->left = arg->left->func(arg->left, bstate);
    obj->value.str_value = arg->value.str_value;

    return obj;
}

Bobject *bt_return(Bcode *code, bcon_State *bstate){
    bstate->return_value = code->left->func(code->left, bstate);
    bstate->islocked = BLOCK_RETURNED;
    return b_None();
}

Bobject *bt_mk_fun(Bcode *code, bcon_State *bstate){
    Bobject *fun = mk_safe_Bobject();
    char *name = (code->value.str_value);
    fun->value.str_value = name;
    fun->type = B_FN;

    // default objects
    Bcode *def_arg = code->left->left;
    Bobject *start_arg = NULL;
    Bobject *recent_arg = NULL;

    // parameter names
    Param *params = NULL;
    Param *recent = NULL;

    while (def_arg != NULL){
        append_param(&params, &recent, mk_param(def_arg));

        if (def_arg->type == OP_MAKE_VARIABLE){
            append_fun_args_data(&start_arg, &recent_arg, mk_def_param(def_arg, bstate));
        }

        def_arg = def_arg->next;
    }

    fun->left = start_arg;
    fun->params = params;
    fun->code = code->right;
    fun->func = bt_exec_function;
    fun->this = b_None();

    add_to_stack(bstate->memory, name, fun);
    return b_None();
}

Bobject* bt_call_function(Bcode* code, bcon_State *bstate){
    Bobject *fun = code->left->func(code->left, bstate);

    // executing given arguments before adding them to stack
    Bobject* start_arg = NULL;
    Bobject* recent_arg = NULL;

    Bcode* raw_args = code->right;

    while (raw_args != NULL) {
        append_fun_args_data(&start_arg, &recent_arg, raw_args->func(raw_args, bstate));
        raw_args = raw_args->next;
    }

    Bobject *return_value = fun->func(start_arg, fun, bstate);
    return return_value;
}

Bobject *bt_exec_function(Bobject *args, Bobject *fun, bcon_State *bstate){
    Param *params = fun->params;
    Bobject *arg = args;

    add_to_stack(bstate->memory, "this", fun->this);

    while (params != NULL && arg != NULL){
        add_to_stack(bstate->memory, params->value, object_copy(arg));
        params = params->next;
        arg = arg->next;
    }

    block_evaluator_start(fun->code, bstate);
    return b_None();
}

// making function arguments
void append_fun_args_data(Bobject** start, Bobject** recent, Bobject* new_token) {
	if (*start == NULL) {
	    *start = new_token;
        *recent = new_token;
    } else {
        (*recent)->next = new_token;
        *recent = new_token;
    }
}

Bytec* bytec_read(Bcode* toks){
	Bytec* pls = calloc(sizeof(Bytec), 1);
	pls->tok = toks;

    return pls;
}

Bobject *bt_make_b_fun(Bobject *(*fun)(Bobject *, Bobject *, bcon_State *)){
    Bobject *fn = mk_safe_Bobject();
    fn->func = fun;
	fn->type = B_FN;
	return fn;
}
