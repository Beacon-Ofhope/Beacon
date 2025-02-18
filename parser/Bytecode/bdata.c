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
    Bobject* bin = mk_Bobject(bstate);
    bin->value.str_value = strdup(code->value.name);
    bin->type = BSTRING;

    return bin;
}

Bobject *bt_none(Bcode *code, bcon_State *bstate){
    Bobject *none = mk_Bobject(bstate);
    none->refs = 100000;
    none->type = BNONE;

    return none;
}

Bobject *bt_num(Bcode *code, bcon_State *bstate){
    Bobject* number = mk_Bobject(bstate);
	number->value.num_value = code->value.number;
	number->type = BNUMBER;
    number->refs = 0;

	return number;
}

Bobject *bt_add_num(Bcode *code, bcon_State *bstate){
    opbinop *op = code->value.binop;
    Bobject *a = op->left->func(op->left, bstate);

    if (bstate->islocked == BLOCK_ERRORED)
        return bstate->none;

    Bobject *b = op->right->func(op->right, bstate);

    if (bstate->islocked == BLOCK_ERRORED)
        return bstate->none;

    double ans = a->value.num_value + b->value.num_value;
    return num_malloc(a, b, ans);
}

Bobject *bt_minus_num(Bcode *code, bcon_State *bstate){
    opbinop *op = code->value.binop;

    Bobject *a = op->left->func(op->left, bstate);
    Bobject *b = op->right->func(op->right, bstate);

    double ans = a->value.num_value - b->value.num_value;
    return num_malloc(a, b, ans);
}

Bobject *bt_div_num(Bcode *code, bcon_State *bstate){
    opbinop *op = code->value.binop;

    Bobject *a = op->left->func(op->left, bstate);
    Bobject *b = op->right->func(op->right, bstate);

    double ans = a->value.num_value / b->value.num_value;
    return num_malloc(a, b, ans);
}

Bobject *bt_mult_num(Bcode *code, bcon_State *bstate){
    opbinop *op = code->value.binop;

    Bobject *a = op->left->func(op->left, bstate);
    Bobject *b = op->right->func(op->right, bstate);

    double ans = a->value.num_value * b->value.num_value;
    return num_malloc(a, b, ans);
}

Bobject *bt_mod_num(Bcode *code, bcon_State *bstate){
    opbinop *op = code->value.binop;

    Bobject *a = op->left->func(op->left, bstate);
    Bobject *b = op->right->func(op->right, bstate);

    double ans = fmod(a->value.num_value, b->value.num_value);
    return num_malloc(a, b, ans);
}

Bobject *bt_less_than_num(Bcode *code, bcon_State *bstate){
    opbinop *op = code->value.binop;

    Bobject *a = op->left->func(op->left, bstate);
    Bobject *b = op->right->func(op->right, bstate);

    double ans = a->value.num_value < b->value.num_value;
    return num_malloc(a, b, ans);
}

Bobject *bt_greater_than_num(Bcode *code, bcon_State *bstate){
    opbinop *op = code->value.binop;

    Bobject *a = op->left->func(op->left, bstate);
    Bobject *b = op->right->func(op->right, bstate);

    double ans = a->value.num_value > b->value.num_value;
    return num_malloc(a, b, ans);
}

Bobject *bt_less_equals_num(Bcode *code, bcon_State *bstate){
    opbinop *op = code->value.binop;

    Bobject *a = op->left->func(op->left, bstate);
    Bobject *b = op->right->func(op->right, bstate);

    double ans = a->value.num_value <= b->value.num_value;
    return num_malloc(a, b, ans);
}

Bobject *bt_greater_equals_num(Bcode *code, bcon_State *bstate){
    opbinop *op = code->value.binop;

    Bobject *a = op->left->func(op->left, bstate);
    Bobject *b = op->right->func(op->right, bstate);

    double ans = a->value.num_value >= b->value.num_value;
    return num_malloc(a, b, ans);
}

Bobject *bt_not(Bcode *code, bcon_State *bstate){
    Bobject *opt = mk_Bobject(bstate);
    opt->value.num_value = !(code->func(code, bstate)->value.num_value);
    opt->type = BNUMBER;
    return opt;
}

Bobject *bt_equals(Bcode *code, bcon_State *bstate){
    opbinop *op = code->value.binop;

    Bobject *a = op->left->func(op->left, bstate);
    Bobject *b = op->right->func(op->right, bstate);

    int ans = 0;

    if (a->type != b->type)
        return data_malloc(a, b, ans);

    switch (a->type){
        case BNUMBER:
            ans = (a->value.num_value == b->value.num_value);
            break;
        case BSTRING:
            if (strcmp(a->value.str_value, b->value.str_value) == 0)
                ans = 1;
            break;
        default:
            if (memcmp(a, b, sizeof(a)) == 0)
                ans = 1;
            break;
    }

    return data_malloc(a, b, ans);
}

Bobject *bt_not_equals(Bcode *code, bcon_State *bstate){
    opbinop *op = code->value.binop;

    Bobject *a = op->left->func(op->left, bstate);
    Bobject *b = op->right->func(op->right, bstate);

    int ans = 0;

    if (a->type != b->type)
        return data_malloc(a, b, 1);

    switch (a->type){
        case BNUMBER:
            ans = (a->value.num_value != b->value.num_value);
            break;
        case BSTRING:
            if (strcmp(a->value.str_value, b->value.str_value))
                ans = 1;
            break;
        default:
            if (memcmp(a, b, sizeof(a)))
                ans = 1;
            break;
    }

    return data_malloc(a, b, ans);
}

Bobject *num_malloc(Bobject *a, Bobject *b, double ans){
    if (a->refs == 0){
        a->value.num_value = ans;

        if (b->refs == 0){
            free_object(b);
            return a;
        }
    }

    if (b->refs == 0){
        b->value.num_value = ans;
        return b;
    }

    Bobject *result = malloc(sizeof(Bobject));
    result->value.num_value = ans;
    result->type = BNUMBER;
    result->refs = 0;

    return result;
}

Bobject *data_malloc(Bobject *a, Bobject *b, int ans){
    if (a->refs == 0) {
        a->value.num_value = ans;
        a->type = BNUMBER;

        if (b->refs == 0){
            free_object(b);
            return a;
        }
    }

    if (b->refs == 0){
        b->value.num_value = ans;
        b->type = BNUMBER;
        return b;
    }

    Bobject *result = malloc(sizeof(Bobject));
    result->value.num_value = ans;
    result->type = BNUMBER;
    result->refs = 0;

    return result;
}

