#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/types/siginfo_t.h>

#include "../Includes/bcode.h"
#include "../Includes/bobject.h"
#include "../Includes/bytecode.h"

Bobject *bt_make_variable(Bcode* code, bcon_State *bstate){
	add_to_stack(bstate->memory, code->value.str_value, code->left->func(code->left, bstate));
	return b_None();
}

Bobject *bt_if(Bcode *code, bcon_State *bstate){
    Bcode *start = code->left;

    while (start != NULL && (bstate->islocked == BLOCK_ISRUNNING)) {
        if (start->left->func(start->left, bstate)->value.num_value != 0){
            block_evaluator_start(start->right, bstate);
            break;
        }
        start = start->next;
    }
    return b_None();
}

Bobject *bt_while(Bcode *code, bcon_State *bstate){
    Bcode *left = code->left;
    Bcode *block_code = code->right;

    Bcode *start;

    while ((bstate->islocked == BLOCK_ISRUNNING) && left->func(left, bstate)->value.num_value != 0){
        start = block_code;

        while (start != NULL && bstate->islocked == BLOCK_ISRUNNING){
            start->func(start, bstate);
            start = start->next;
        }

        if (bstate->islocked == BLOCK_CONTINUE)
            bstate->islocked = BLOCK_ISRUNNING;
    }

    if (bstate->islocked == BLOCK_BROKE)
        bstate->islocked = BLOCK_ISRUNNING;

    return b_None();
}

Bobject *bt_break(Bcode *code, bcon_State *bstate){
    bstate->islocked = BLOCK_BROKE;
    return b_None();
}

Bobject *bt_continue(Bcode *code, bcon_State *bstate){
    bstate->islocked = BLOCK_CONTINUE;
    return b_None();
}

Bobject *bt_get_attribute(Bcode *code, bcon_State *bstate){
    Bobject *obj = code->left->func(code->left, bstate);
    Bobject *fetched = get_from_stack(obj->attrs, code->value.str_value);

    if (fetched == NULL) {
        printf("AttributeError: '%s' is not an attribute.\n", code->value.str_value);
        exit(0);
    }

    return fetched;
}

Bobject *bt_set_attribute(Bcode *code, bcon_State *bstate){
    Bobject *obj = code->left->func(code->left, bstate);
    add_to_stack(obj->attrs, code->value.str_value, code->right->func(code->right, bstate));
    
    return b_None();
}

Bobject *bt_get_variable(Bcode *code, bcon_State *bstate){
    Bobject *fetched = get_from_stack(bstate->memory, code->value.str_value);

    if (fetched == NULL) {
        printf("UndefinedError: '%s' is not defined\n       at line: %d\n", code->value.str_value, code->line);
        exit(0);
    }

    return fetched;
}
