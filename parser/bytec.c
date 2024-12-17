#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../Include/bapi.h"
#include "bcode.h"
#include "bobject.h"
#include "bytec.h"

#include <bits/types/siginfo_t.h>
#include <ctype.h>
#include "../Include/helpers.h"


Bobject* mk_safe_Bobject(){
    Bobject* obj = malloc(sizeof(Bobject));
    obj->next = NULL;
    return obj;
}

Bobject* bt_make_variable(Bcode* code, Stack* stack){
	add_to_stack(stack, code->value.str_value, code->left->func(code->left, stack));
	return b_None();
}

Bobject* bt_str(Bcode* code, Stack* stack){
	Bobject* str_value = mk_safe_Bobject();
    str_value->value.str_value = code->value.str_value;
    str_value->type = B_STR;
    return str_value;
}

Bobject * bt_get_variable(Bcode* code, Stack* stack){
    const Bobject * fetched = get_from_stack(stack, code->value.str_value);

    if (fetched == NULL) {
        printf("UndefinedError: '%s' is not defined\n", code->value.str_value);
        exit(0);
    }

    Bobject * value = malloc(sizeof(*fetched));
    memcpy(value, fetched, sizeof(*fetched));
    return value;
}

Bobject * bt_get_attribute(Bcode* code, Stack* stack){
    Bobject *obj = code->left->func(code->left, stack);
    Bobject *fetched = get_from_stack(obj->attrs, code->value.str_value);

    if (fetched == NULL) {
        printf("AttributeError: '%s' is not an attribute.\n", code->value.str_value);
        exit(0);
    }

    return fetched;
}


Bobject* bt_num(Bcode *code, Stack *stack) {
	Bobject* num_value = mk_safe_Bobject();

	num_value->value.num_value = code->value.num_value;
	num_value->type = B_NUM;
	return num_value;
}

Bobject* bt_add_num(Bcode* code, Stack* stack){
	Bobject* number = mk_safe_Bobject();
    number->value.num_value = (code->left->func(code->left, stack)->value.num_value +
								code->right->func(code->right, stack)->value.num_value);

    number->type = B_NUM;
	return number;
}

Bobject* bt_minus_num(Bcode *code, Stack *stack){
    Bobject *number = mk_safe_Bobject();
    number->value.num_value = (code->left->func(code->left, stack)->value.num_value -
                               code->right->func(code->right, stack)->value.num_value);
    number->type = B_NUM;
    return number;
}

Bobject* bt_div_num(Bcode *code, Stack *stack){
    Bobject* number = mk_safe_Bobject();
    number->value.num_value = (code->left->func(code->left, stack)->value.num_value /
                               code->right->func(code->right, stack)->value.num_value);
    number->type = B_NUM;
    return number;
}

Bobject *bt_mult_num(Bcode *code, Stack *stack){
    Bobject *number = mk_safe_Bobject();
    number->value.num_value = (code->left->func(code->left, stack)->value.num_value *
                               code->right->func(code->right, stack)->value.num_value);
    number->type = B_NUM;
    return number;
}

Bobject *bt_mod_num(Bcode *code, Stack *stack){
    Bobject *number = mk_safe_Bobject();
    number->value.num_value = fmod(code->left->func(code->left, stack)->value.num_value,
                               code->right->func(code->right, stack)->value.num_value);
    number->type = B_NUM;
    return number;
}

//// COLLECTING ARGUMENTS FUNCTIONS
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
	Bytec* pls = malloc(sizeof(Bytec));
	pls->tok = toks;

    return pls;
}

Bobject* bt_call_function(Bcode* code, Stack* stack){
    Bobject *fun = get_from_stack(stack, code->value.str_value);

    if (fun == NULL){
        fprintf(stderr, "'%s' is not defined", code->value.str_value);
        exit(0);
    }

    // executing given arguments before adding them to stack
    Bobject* start_arg = NULL;
    Bobject* recent_arg = NULL;

    Bcode* raw_args = code->left;

    while (raw_args != NULL) {
        append_fun_args_data(&start_arg, &recent_arg, raw_args->func(raw_args, stack));
        raw_args = raw_args->next;
    }

    fun->func(start_arg, fun, stack);
    return b_None();
}

Bobject* bt_make_b_fun(Bobject * (*fun)(Bobject *, Bobject *, Stack*)){
    Bobject *fn = mk_safe_Bobject();
    fn->func = fun;
	fn->type = B_FN;
	return fn;
}

Bobject* bt_list(Bcode *code, Stack *stack){
    Bobject *start_data = NULL;
    Bobject *recent_data = NULL;

    Bcode *raw_data = code->left;
    double len = 0;

    while (raw_data != NULL){
        append_fun_args_data(&start_data, &recent_data, raw_data->func(raw_data, stack));
        raw_data = raw_data->next;
        len++;
    }

    Bobject* list = mk_safe_Bobject();
    list->type = B_LIST;
    list->left = start_data;
    list->value.num_value = len;

    return list;
}

// executing code blocks and statements
void block_evaluator_start(Bcode *run, Stack *stack){
    Bcode *start = run;
    while (start != NULL) {
        start->func(start, stack);
        start = start->next;
    }
}

Bobject *bt_if(Bcode *code, Stack *stack){
    Bcode *start = code->left;

    while (start != NULL) {
        if (start->left->func(start->left, stack)->value.num_value != 0){
            block_evaluator_start(start->right, stack);
            break;
        }
        start = start->next;
    }
    return b_None();
}

Bobject *bt_while(Bcode *code, Stack *stack) {
    while (code->left->func(code->left, stack)->value.num_value != 0){
        block_evaluator_start(code->right, stack);
    }

    return b_None();
}

Param *mk_param(Bcode *code){
    char *name = copy_string_safely(code->value.str_value);
    Param *param = malloc(sizeof(Param));
    param->next = NULL;
    param->value = name;

    return param;
}

void append_param(Param** start, Param** recent, Param* new_token) {
	if (*start == NULL) {
	    *start = new_token;
        *recent = new_token;
    } else {
        (*recent)->next = new_token;
        *recent = new_token;
    }
}

Bobject *mk_def_param(Bcode *arg, Stack *stack){
    Bobject *obj = mk_safe_Bobject();
    obj->left = arg->left->func(arg->left, stack);
    obj->value.str_value = copy_string_safely(arg->value.str_value);

    return obj;
}

Bobject *object_copy(Bobject *copy){
    Bobject *obj = mk_safe_Bobject();
    obj->type = copy->type;
    obj->value = copy->value;
    obj->params = copy->params;
    obj->func = copy->func;
    obj->code = copy->code;

    return obj;
}

Bobject *bt_exec_function(Bobject *args, Bobject *fun, Stack *stack){
    Param *params = fun->params;
    Bobject *arg = args;

    while (params != NULL && arg != NULL){

        add_to_stack(stack, params->value, object_copy(arg));
        params = params->next;
        arg = arg->next;
    }

    block_evaluator_start(fun->code, stack);
    return b_None();
}

Bobject *bt_mk_fun(Bcode *code, Stack *stack){
    Bobject *fun = mk_safe_Bobject();
    char *name = copy_string_safely(code->value.str_value);
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

        if (def_arg->type == I_VAR_MAKE){
            append_fun_args_data(&start_arg, &recent_arg, mk_def_param(def_arg, stack));
        }

        def_arg = def_arg->next;
    }

    fun->left = start_arg;
    fun->params = params;
    fun->code = code->right;
    fun->func = bt_exec_function;

    add_to_stack(stack, name, fun);
    return b_None();
}

Bobject *bt_mk_class(Bcode *code, Stack *stack){
    Bobject *cls = mk_safe_Bobject();
    char *name = copy_string_safely(code->value.str_value);
    cls->value.str_value = name;
    cls->type = B_TYPE;

    // parameter names
    Param *params = NULL;
    Param *recent = NULL;

    // extends
    Bcode *exts = code->left->left;

    while (exts != NULL){
        append_param(&params, &recent, mk_param(exts));

        exts = exts->next;
    }

    cls->params = params;
    cls->code = code->right;
    cls->func = bt_exec_function;

    cls->attrs = mk_type(stack, code, name);

    add_to_stack(stack, name, cls);
    return b_None();
}

Bobject *bt_mk_string(char *value){
    Bobject *str_value = mk_safe_Bobject();
    str_value->value.str_value = value;
    str_value->type = B_STR;
    return str_value;
}

void save_type_object(Bcode *code, Stack *stack, Stack *attr){
    Bcode *prop = code->right;
    int _type;

    block_evaluator_start(code->right, stack);

    // saving to object attributes
    while (prop != NULL){
        _type = prop->type;

        if (_type == I_FN || _type == I_VAR_MAKE){
            add_to_stack(attr, prop->value.str_value, get_from_stack(stack, prop->value.str_value));
        }

        prop = prop->next;
    }
}

Stack *mk_type(Stack *stack, Bcode *code, char *name){
    Stack *attrs = create_stack();
    add_to_stack(attrs, "_name", bt_mk_string(name));
    add_to_stack(attrs, "_file", bt_mk_string((get_from_stack(stack, "_file"))->value.str_value));

    save_type_object(code, stack, attrs);

    return attrs;
}
