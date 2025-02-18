#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/types/siginfo_t.h>


#include "../Includes/bcode.h"
#include "../Includes/bobject.h"
#include "../Includes/bytecode.h"
#include "../../Modules/Includes/_imports.h"

// INTERFACE IMPLEMENTATION CODE
Bobject *bt_face(Bcode *code, bcon_State *bstate){
    opdict *dt = code->value.dict;
    Stack* attrs = create_stack((int)(code->line));

    char **keys = dt->keys;
    Bcode *value = dt->values;

    int count = code->line;
    Bobject *val;

    for (int i = 0; i < count; i++) {
        val = value->func(value, bstate);

        if (bstate->islocked != BLOCK_ISRUNNING)
            return bstate->none;

        add_to_stack_i(attrs, keys[i], val);
        value = value->next;
    }

    binterface *bin = malloc(sizeof(binterface));
    bin->count = count;
    bin->attrs = attrs;

    Bobject *module = mk_safe_Bobject();
    module->type = BINTERFACE;
    module->value.bface = bin;

    return module;
}

// CLASS IMPLEMENTATION CODE
Stack *mk_type(bcon_State *bstate, char *name){
    Stack *props = create_stack(4);
    // add_to_stack(props, "__name__", bt_mk_string(name));

    return props;
}

Bobject *bt_mk_class(Bcode *code, bcon_State *bstate){
    opmkclass *cl = code->value.pmkclass;

    btype *bty = malloc(sizeof(btype));
    bty->bstate = bstate;
    bty->func = bt_exec_type;
    bty->code = cl->code_block;
    bty->attrs = mk_type(bstate, cl->name);
    bty->name = cl->name;

    if (bstate->islocked != BLOCK_ISRUNNING)
        return bstate->none;

    Bobject *cls = mk_Bobject(bstate);
    cls->value.btype = bty;
    cls->type = BTYPE;

    add_to_stack(bstate->callStack[bstate->stackPos], cl->name, cls);
    return bstate->none;
}

Bobject *mk_object(Bobject *obj, btype *fun) {
    bcon_State *bstate = fun->bstate;
    Stack *attributes = create_stack(0);

    Stack *original = bstate->memory;
    Stack *dup = copy_stack(original);
    
    bstate->memory = dup;
    block_evaluator_start(fun->code, bstate);

    // saving to object attributes
    Bobject* recent = NULL;
    Bobject* constructor = NULL;

    Bcode *code = fun->code;

    while (code != NULL) {
        switch (code->type) {
            case OP_FUNCTION:
                char *name = code->value.pmkfun->name;
                recent = get_from_stack(bstate->memory, name);

                if (recent->type ==  BFUNCTION) {
                    add_to_stack(attributes, name, recent);
                    recent->value.bfun->this = obj;

                    if (strcmp(name, "constructor") == 0)
                        constructor = recent;                 
                }
                break;

            case OP_MAKE_VARIABLE:
                char *var = code->value.mkvar->key;
                add_to_stack(attributes, var, get_from_stack(bstate->memory, var));
                break;

            default:
                break;
        }
        code = code->next;
    }

    obj->value.bclass->attrs = attributes;
    bstate->memory = original;

    return constructor;
}

Bobject *bt_exec_type(bargs *args, btype *fun, bcon_State *bstate){
    bclass* cls = malloc(sizeof(bclass));
    cls->name = fun->name;

    Bobject *obj = mk_Bobject(bstate);
    obj->value.bclass = cls;
    obj->type = BOBJECT;

    /*
        EXTENDED CODE
        GOES HERE
    */
   
    Bobject *constructor = mk_object(obj, fun);

    if (constructor != NULL){
        bfunction *bf = constructor->value.bfun;
        bf->func(args, bf, bstate);
    }

    return obj;
}


