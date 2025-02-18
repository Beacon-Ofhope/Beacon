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


Bobject *bt_make_variable(Bcode* code, bcon_State *bstate){
    opmkvar *var = code->value.mkvar;
    Bobject* value = var->value->func(var->value, bstate);

    if (bstate->islocked == BLOCK_ISRUNNING)
        add_to_stack(bstate->callStack[bstate->stackPos], var->key, value);

	return bstate->none;
}

Bobject *bt_get_variable(Bcode *code, bcon_State *bstate){
    Bobject *var = NULL;

    for (int i = bstate->stackPos; (i >= 0 && var == NULL); --i)
        var = get_from_stack(bstate->callStack[i], code->value.name);

    if (var == NULL)
        return _reference_error(bstate, code->value.name, code->line);

    return var;
}


Bobject *bt_if(Bcode *code, bcon_State *bstate){
    opwhile **pifs = code->value.pif;
    int count = code->line;

    opwhile *each_if;
    Bcode *condition;

    for(int i = 0; (i < count && (bstate->islocked == BLOCK_ISRUNNING)); ++i){
        each_if = pifs[i];
        condition = each_if->condition;

        if ((condition->func(condition, bstate)->value.num_value) && (bstate->islocked == BLOCK_ISRUNNING)){
            block_evaluator_start(each_if->code_block, bstate);
            break;
        }
    }

    return bstate->none;
}

Bobject *bt_while(Bcode *code, bcon_State *bstate){
    opwhile *wl = code->value.pwhile;

    Bcode *code_block = wl->code_block;
    Bcode *condition = wl->condition;
    Bcode *start;

    while ((bstate->islocked == BLOCK_ISRUNNING) && condition->func(condition, bstate)->value.num_value){
        start = code_block;

        while (start != NULL && bstate->islocked == BLOCK_ISRUNNING){
            start->func(start, bstate);
            start = start->next;
        }

        if (bstate->islocked == BLOCK_CONTINUE)
            bstate->islocked = BLOCK_ISRUNNING;
    }

    if (bstate->islocked == BLOCK_BROKE)
        bstate->islocked = BLOCK_ISRUNNING;

    return bstate->none;
}

Bobject *bt_break(Bcode *code, bcon_State *bstate){
    bstate->islocked = BLOCK_BROKE;
    return bstate->none;
}

Bobject *bt_continue(Bcode *code, bcon_State *bstate){
    bstate->islocked = BLOCK_CONTINUE;
    return bstate->none;
}

Bobject *bt_get_attribute(Bcode *code, bcon_State *bstate){
    opgattr *gt = code->value.pget;
    Bobject *obj = gt->parent->func(gt->parent, bstate);

    if (bstate->islocked != BLOCK_ISRUNNING)
        return bstate->none;

    Bobject *get = NULL;

    switch (obj->type){
        case BOBJECT:
            get = get_from_stack(obj->value.bclass->attrs, gt->key);
            break;
        case BMODULE:
            get = get_from_stack(obj->value.module->attrs, gt->key);
            break;
        case BINTERFACE:
            get = get_from_stack(obj->value.bface->attrs, gt->key);
            break;
        case BTYPE:
            get = get_from_stack(obj->value.btype->attrs, gt->key);
            break;
        default:
            return _attribute_error(bstate, obj, code->line);
    }

    if (get == NULL)
        return _no_attribute_error(bstate, obj, gt->key, code->line);
    return get;
}

Bobject *bt_set_attribute(Bcode *code, bcon_State *bstate){
    opsattr *gt = code->value.pset;
    Bobject *obj = gt->parent->func(gt->parent, bstate);

    if (bstate->islocked != BLOCK_ISRUNNING)
        return bstate->none;

    Bobject *value = gt->value->func(gt->value, bstate);

    if (bstate->islocked == BLOCK_ISRUNNING) {
        switch (obj->type){
            case BOBJECT:
                add_to_stack(obj->value.bclass->attrs, gt->key, value);
                break;
            case BMODULE:
                add_to_stack(obj->value.module->attrs, gt->key, value);
                break;
            case BINTERFACE:
                add_to_stack(obj->value.bface->attrs, gt->key, value);
                break;
            case BTYPE:
                add_to_stack(obj->value.btype->attrs, gt->key, value);
                break;
            default:
                return _set_attribute_error(bstate, obj, code->line);
        }
    }
    return bstate->none;
}

Bobject *bt_try(Bcode *code, bcon_State *bstate){
    optry *ty = code->value.ptry;
    block_evaluator_start(ty->tried, bstate);

    if (bstate->islocked != BLOCK_ERRORED)
        return bstate->none;

    bstate->islocked = BLOCK_ISRUNNING;

    if (ty->retry){
        if (ty->error_name)
            add_to_stack(bstate->memory, ty->error_name, bstate->return_value);

        block_evaluator_start(ty->retry, bstate);
    }

    return bstate->none;
}

Bobject *bt_throw(Bcode *code, bcon_State *bstate){
    Bcode *th = code->value.data;
    bstate->return_value = th->func(th, bstate);

    bstate->islocked = BLOCK_ERRORED;
    return bstate->none;
}


