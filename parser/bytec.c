#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "dcode.h"
#include "bytec.h"
#include "dobject.h"


Dobject* bt_make_variable(Dcode* code, Stack* stack){
	Dobject* old_data = code->left->func(code->left, stack);
	Dobject* new_assign = (Dobject*)malloc(sizeof(Dobject));

	new_assign->type = old_data->type;
	new_assign->value = old_data->value;
	new_assign->func = old_data->func;

	add_to_stack(stack, code->value.str_value, new_assign);
	// print_stack(stack);
}

Dobject* bt_str(Dcode* code, Stack* stack){
	Dobject* str_value = (Dobject*)malloc(sizeof(Dobject));

	str_value->value.str_value = code->value.str_value;
	str_value->type = D_STR;
	return str_value;
}

Dobject* bt_get_variable(Dcode* code, Stack* stack){
	return get_from_stack(stack, code->value.str_value);
}

Dobject* bt_num(Dcode* code, Stack* stack){
	Dobject* num_value = (Dobject*)malloc(sizeof(Dobject));

	num_value->value.num_value = code->value.num_value;
	num_value->type = D_NUM;
	return num_value;
}

Dobject* bt_add_num(Dcode* code, Stack* stack){
	Dobject* number = (Dobject*)malloc(sizeof(Dobject));

	number->value.num_value = (code->left->func(code->left, stack)->value.num_value + 
								code->left->func(code->right, stack)->value.num_value);
	number->type = D_NUM;
	return number;
}

Dobject* bt_div_num(Dcode* code, Stack* stack){
	Dobject* number = (Dobject*)malloc(sizeof(Dobject));

	number->value.num_value = (code->left->func(code->left, stack)->value.num_value / 
								code->left->func(code->right, stack)->value.num_value);
	number->type = D_NUM;
	return number;
}

Dobject* bt_sub_num(Dcode* code, Stack* stack){
	Dobject* number = (Dobject*)malloc(sizeof(Dobject));

	number->value.num_value = (code->left->func(code->left, stack)->value.num_value - 
								code->left->func(code->right, stack)->value.num_value);
	number->type = D_NUM;
	return number;
}

Dobject* bt_mult_num(Dcode* code, Stack* stack){
	Dobject* number = (Dobject*)malloc(sizeof(Dobject));

	number->value.num_value = (code->left->func(code->left, stack)->value.num_value * 
								code->left->func(code->right, stack)->value.num_value);
	number->type = D_NUM;
	return number;
}

Dobject* bt_modulus_num(Dcode* code, Stack* stack){
	Dobject* number = (Dobject*)malloc(sizeof(Dobject));

	number->value.num_value = fmod(code->left->func(code->left, stack)->value.num_value,  
								code->left->func(code->right, stack)->value.num_value);
	number->type = D_NUM;
	return number;
}

Dobject* bt_less_than(Dcode* code, Stack* stack){
	Dobject* number = (Dobject*)malloc(sizeof(Dobject));

	number->value.num_value = (code->left->func(code->left, stack)->value.num_value * 
								code->left->func(code->right, stack)->value.num_value);
	number->type = D_NUM;
	return number;
}

Dobject* bt_greater_than(Dcode* code, Stack* stack){
	Dobject* number = (Dobject*)malloc(sizeof(Dobject));

	number->value.num_value = (code->left->func(code->left, stack)->value.num_value * 
								code->left->func(code->right, stack)->value.num_value);
	number->type = D_NUM;
	return number;
}

Dobject* bt_less_equal_to(Dcode* code, Stack* stack){
	Dobject* number = (Dobject*)malloc(sizeof(Dobject));

	number->value.num_value = (code->left->func(code->left, stack)->value.num_value * 
								code->left->func(code->right, stack)->value.num_value);
	number->type = D_NUM;
	return number;
}

Dobject* bt_greater_equal_to(Dcode* code, Stack* stack){
	Dobject* number = (Dobject*)malloc(sizeof(Dobject));

	number->value.num_value = (code->left->func(code->left, stack)->value.num_value * 
								code->left->func(code->right, stack)->value.num_value);
	number->type = D_NUM;
	return number;
}

Dobject* bt_not_equal_to(Dcode* code, Stack* stack){
	Dobject* number = (Dobject*)malloc(sizeof(Dobject));

	number->value.num_value = (code->left->func(code->left, stack)->value.num_value * 
								code->left->func(code->right, stack)->value.num_value);
	number->type = D_NUM;
	return number;
}

// COLLECTING ARGUMENTS FUNCTIONS
void appendDobject_args(Dobject** head, Dobject* newToken) {
	if (*head == NULL) {
        *head = newToken;
    } else {
        Dobject* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newToken;
    }

}

Bytec* bytec_read(Dcode* toks){
	Bytec* pls = malloc(sizeof(struct BYTEC_UP));
	pls->tok = toks;
}

void bytec_advance(Bytec* pls){
	if (pls->tok != NULL)
		pls->tok = pls->tok->next;
}

Dobject* bt_call_function(Dcode* code, Stack* stack){
	Dobject* fn = get_from_stack(stack, code->value.str_value);
	Dobject* args_head = NULL;

	Bytec* pls = bytec_read(code->left);
	char number_of_args = 0;

	while (pls->tok != NULL){
		appendDobject_args(&args_head, pls->tok->func(pls->tok, stack));

		number_of_args++;
		bytec_advance(pls);
	}

	Dobject* res = fn->func(args_head, stack);
	return res;
}

Dobject* bt_make_b_function(Dobject* (*fun)(Dobject *, Stack*)){
	Dobject* fn = (Dobject*)malloc(sizeof(Dobject));
	fn->func = fun;
	fn->type = D_FN;
	return fn;
}


// COLLECTING FUNCTION PARAMETERS
void appendParameter(Param** head, Param* newToken) {
	if (*head == NULL) {
        *head = newToken;
    }
    else {
        Param* current = *head;
        while (current->next != NULL)
            current = current->next;
        current->next = newToken;
    }
}

Dobject* exec_function(Dobject* fun, Stack* stack){

}


Dobject* bt_make_function(Dcode* code, Stack* stack){
	Dobject* fun = (Dobject*)malloc(sizeof(Dobject));
	fun->type = D_FN;
	// fun->func = &exec_function;

	Bytec* pls = bytec_read(code->left);
	char number_of_parameters = 0;

	Param* parameters_head = NULL;
	Dobject* defaults_head = NULL;

	while (pls->tok != NULL){
		// safely add parameters to string list
		Param param_value = {pls->tok->value.str_value};
		appendParameter(&parameters_head, &param_value);

		if (pls->tok->type == I_VAR_MAKE) {
			Dobject* ret = pls->tok->left->func(pls->tok->left, stack);
			// appendDobject_args(&defaults_head, ret);
		}

		bytec_advance(pls);
	}

	fun->params = parameters_head;
	fun->default_data = defaults_head;
	// fun->func = ;

	add_to_stack(stack, code->value.str_value, fun);
}
