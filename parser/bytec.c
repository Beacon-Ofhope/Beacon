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
    str_value->value.str_value = copy_string_safely(code->value.str_value);
    str_value->type = B_STR;
    return str_value;
}

Bobject * bt_get_variable(Bcode* code, Stack* stack){
    const Bobject * fetched = get_from_stack(stack, code->value.str_value);

    if (fetched == NULL) {
        printf("UndefinedError: '%s' is not defined", code->value.str_value);
        exit(0);
    }

    Bobject * value = malloc(sizeof(*fetched));
    memcpy(value, fetched, sizeof(*fetched));
    return value;
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

//void bytec_advance(Bytec* pls){
//	if (pls->tok != NULL)
//		pls->tok = pls->tok->next;
//}

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
        if (raw_args->type == I_VAR_MAKE) {

        } else {
            append_fun_args_data(&start_arg, &recent_arg, raw_args->func(raw_args, stack));
        }
        raw_args = raw_args->next;
    }

    fun->func(start_arg, stack);
    return b_None();
}

Bobject* bt_make_b_fun(Bobject * (*fun)(Bobject  *, Stack*)){
	Bobject * fn = mk_safe_Bobject();
	fn->func = fun;
	fn->type = B_FN;
	return fn;
}
//
//
//// COLLECTING FUNCTION PARAMETERS
//void appendParameter(Param** head, Param* newToken) {
//	if (*head == NULL) {
//        *head = newToken;
//    }
//    else {
//        Param* current = *head;
//        while (current->next != NULL)
//            current = current->next;
//        current->next = newToken;
//    }
//}
//
//Dobject* exec_function(Dobject* fun, Stack* stack){
//
//}
//
//
//Dobject* bt_make_function(Dcode* code, Stack* stack){
//	Dobject* fun = (Dobject*)malloc(sizeof(Dobject));
//	fun->type = D_FN;
//	// fun->func = &exec_function;
//
//	Bytec* pls = bytec_read(code->left);
//	char number_of_parameters = 0;
//
//	Param* parameters_head = NULL;
//	Dobject* defaults_head = NULL;
//
//	while (pls->tok != NULL){
//		// safely add parameters to string list
//		Param param_value = {pls->tok->value.str_value};
//		appendParameter(&parameters_head, &param_value);
//
//		if (pls->tok->type == I_VAR_MAKE) {
//			Dobject* ret = pls->tok->left->func(pls->tok->left, stack);
//			// appendDobject_args(&defaults_head, ret);
//		}
//
//		bytec_advance(pls);
//	}
//
//	fun->params = parameters_head;
//	fun->default_data = defaults_head;
//	// fun->func = ;
//
//	add_to_stack(stack, code->value.str_value, fun);
//}

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

Bobject *bt_if(Bcode *code, Stack *stack){

    return b_None();
}
