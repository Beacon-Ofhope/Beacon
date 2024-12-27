#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/types/siginfo_t.h>

#include "../Includes/bcode.h"
#include "../Includes/bobject.h"
#include "../Includes/bytecode.h"


Bobject *bt_mk_class(Bcode *code, bcon_State *bstate){
    Bobject *cls = mk_safe_Bobject();
    char *name = code->value.str_value;
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
    cls->code = code;
    cls->func = bt_exec_type;

    cls->attrs = mk_type(bstate, code, name);

    add_to_stack(bstate->memory, name, cls);
    return b_None();
}

Stack *mk_type(bcon_State *bstate, Bcode *code, char *name){
    Stack *attrs = create_stack();
    add_to_stack(attrs, "_name", bt_mk_string(name));
    add_to_stack(attrs, "_file", bt_mk_string((get_from_stack(bstate->memory, "_file"))->value.str_value));

    save_type_block(code, bstate, attrs);

    return attrs;
}

// makes the type functions from bytecode
void save_type_block(Bcode *code, bcon_State *bstate, Stack *attr){
    Bcode *prop = code->right;
    int _type;

    block_evaluator_start(code->right, bstate);

    // saving to object attributes
    while (prop != NULL){
        _type = prop->type;

        if (_type == OP_FUNCTION || _type == OP_MAKE_VARIABLE)
            add_to_stack(attr, prop->value.str_value, get_from_stack(bstate->memory, prop->value.str_value));

        prop = prop->next;
    }
}

// makes the object functions from bytecodes
// appends the self to the class function
void save_object_block(Bobject *cls, Bcode *code, bcon_State *bstate, Stack *attr){
    Bcode *prop = code->right;
    int _type;

    block_evaluator_start(code->right, bstate);

    // saving to object attributes
    while (prop != NULL){
        _type = prop->type;

        if (_type == OP_FUNCTION || _type == OP_MAKE_VARIABLE)
            add_to_stack(attr, prop->value.str_value, get_from_stack(bstate->memory, prop->value.str_value));

        if (_type == OP_FUNCTION){
            get_from_stack(attr, prop->value.str_value)->this = cls;
        }

        prop = prop->next;
    }
}

Stack *mk_object(Bobject *cls, bcon_State *bstate, Bcode *code, char *name){
    Stack *attrs = create_stack();
    add_to_stack(attrs, "_name", bt_mk_string(name));
    add_to_stack(attrs, "_file", bt_mk_string((get_from_stack(bstate->memory, "_file"))->value.str_value));

    save_object_block(cls, code, bstate, attrs);
    return attrs;
}

Bobject *bt_exec_type(Bobject *args, Bobject *fun, bcon_State *bstate){
    Bobject *cls = mk_safe_Bobject();
    char *name = fun->value.str_value;
    cls->value.str_value = name;
    cls->type = B_OBJECT;

    // parameter names
    Param *params = NULL;
    Param *recent = NULL;

    cls->params = params;
    cls->attrs = mk_object(cls, bstate, fun->code, name);

    return cls;
}


