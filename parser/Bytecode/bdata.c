#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/types/siginfo_t.h>

#include "../Includes/bcode.h"
#include "../Includes/bobject.h"
#include "../Includes/bytecode.h"

Bobject *bt_str(Bcode *code, bcon_State *bstate){
    Bobject* str_value = mk_safe_Bobject();
    str_value->value.str_value = code->value.str_value;
    str_value->type = B_STR;
    return str_value;
}

Bobject *bt_list(Bcode *code, bcon_State *bstate){
    Bobject *start_data = NULL;
    Bobject *recent_data = NULL;

    Bcode *raw_data = code->left;
    double len = 0;

    while (raw_data != NULL){
        append_fun_args_data(&start_data, &recent_data, raw_data->func(raw_data, bstate));
        raw_data = raw_data->next;
        len++;
    }

    Bobject* list = mk_safe_Bobject();
    list->type = B_LIST;
    list->left = start_data;
    list->value.str_value = "list";

    return list;
}

Bobject *bt_mk_string(char *value){
    Bobject *str_value = mk_safe_Bobject();
    str_value->value.str_value = value;
    str_value->type = B_STR;
    return str_value;
}

Bobject *bt_num(Bcode *code, bcon_State *bstate){
    Bobject* num_value = mk_safe_Bobject();

	num_value->value.num_value = code->value.num_value;
	num_value->type = B_NUM;
	return num_value;
}

Bobject *bt_add_num(Bcode *code, bcon_State *bstate){
    Bobject* number = mk_safe_Bobject();
    number->value.num_value = (code->left->func(code->left, bstate)->value.num_value +
								code->right->func(code->right, bstate)->value.num_value);

    number->type = B_NUM;
	return number;
}

Bobject *bt_minus_num(Bcode *code, bcon_State *bstate){
    Bobject *number = mk_safe_Bobject();
    number->value.num_value = (code->left->func(code->left, bstate)->value.num_value -
                               code->right->func(code->right, bstate)->value.num_value);
    number->type = B_NUM;
    return number;
}

Bobject *bt_div_num(Bcode *code, bcon_State *bstate){
    Bobject *number = mk_safe_Bobject();
    number->value.num_value = (code->left->func(code->left, bstate)->value.num_value /
                               code->right->func(code->right, bstate)->value.num_value);
    number->type = B_NUM;
    return number;
}

Bobject *bt_mult_num(Bcode *code, bcon_State *bstate){
    Bobject *number = mk_safe_Bobject();
    number->value.num_value = (code->left->func(code->left, bstate)->value.num_value *
                               code->right->func(code->right, bstate)->value.num_value);
    number->type = B_NUM;
    return number;
}

Bobject *bt_mod_num(Bcode *code, bcon_State *bstate){
    Bobject *number = mk_safe_Bobject();
    number->value.num_value = fmod(code->left->func(code->left, bstate)->value.num_value,
                               code->right->func(code->right, bstate)->value.num_value);
    number->type = B_NUM;
    return number;
}

Bobject *bt_less_than_num(Bcode *code, bcon_State *bstate){
    Bobject *number = mk_safe_Bobject();
    number->value.num_value = (code->left->func(code->left, bstate)->value.num_value <
                               code->right->func(code->right, bstate)->value.num_value);
    number->type = B_NUM;
    return number;
}

Bobject *bt_greater_than_num(Bcode *code, bcon_State *bstate){
    Bobject *number = mk_safe_Bobject();
    number->value.num_value = (code->left->func(code->left, bstate)->value.num_value >
                               code->right->func(code->right, bstate)->value.num_value);
    number->type = B_NUM;
    return number;
}

Bobject *bt_less_equals_num(Bcode *code, bcon_State *bstate){
    Bobject *number = mk_safe_Bobject();
    number->value.num_value = (code->left->func(code->left, bstate)->value.num_value <=
                               code->right->func(code->right, bstate)->value.num_value);
    number->type = B_NUM;
    return number;
}

Bobject *bt_greater_equals_num(Bcode *code, bcon_State *bstate){
    Bobject *number = mk_safe_Bobject();
    number->value.num_value = (code->left->func(code->left, bstate)->value.num_value >=
                               code->right->func(code->right, bstate)->value.num_value);
    number->type = B_NUM;
    return number;
}

Bobject *bt_not(Bcode *code, bcon_State *bstate){
    Bobject *opt = mk_safe_Bobject();
    opt->value.num_value = !(code->left->func(code->left, bstate)->value.num_value);
    opt->type = B_NUM;
    return opt;
}

Bobject *bt_equals(Bcode *code, bcon_State *bstate){
    Bobject* left = code->left->func(code->left, bstate);

    Bobject* number = mk_safe_Bobject();
    number->type = B_NUM;

    switch (left->type){
        case B_NUM:
            number->value.num_value = (left->value.num_value == (code->right->func(code->right, bstate)->value.num_value));
            break;
        case B_STR:
            if (strcmp(left->value.str_value, (code->right->func(code->right, bstate)->value.str_value)) == 0)
                number->value.num_value = 1;
            else
                number->value.num_value = 0;
            break;
        default:
            break;
    }
    return number;
}

Bobject *bt_not_equals(Bcode *code, bcon_State *bstate){
    Bobject* left = code->left->func(code->left, bstate);

    Bobject* number = mk_safe_Bobject();
    number->type = B_NUM;

    switch (left->type){
        case B_NUM:
            number->value.num_value = (left->value.num_value != (code->right->func(code->right, bstate)->value.num_value));
            break;
        case B_STR:
            if (strcmp(left->value.str_value, (code->right->func(code->right, bstate)->value.str_value)) == 0)
                number->value.num_value = 0;
            else
                number->value.num_value = 1;
            break;
        default:
            break;
    }
    return number;
}
