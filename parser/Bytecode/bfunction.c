#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/types/siginfo_t.h>

#include "../Includes/bcode.h"
#include "../Includes/bobject.h"
#include "../Includes/bytecode.h"
#include "../../Modules/Includes/_imports.h"

Bobject *bt_return(Bcode *code, bcon_State *bstate){
    Bcode *rt = code->value.data;

    bstate->return_value = rt->func(rt, bstate);
    bstate->islocked = BLOCK_RETURNED;
    return bstate->none;
}

Bobject *bt_mk_fun(Bcode *code, bcon_State *bstate){
    opmkfun *f = code->value.pmkfun;

    bfunction* fn = malloc(sizeof(bfunction));
    fn->func = bt_exec_function;
    fn->code = f->code_block;
    fn->params = f->params;
    fn->bstate = bstate;
    fn->this = NULL;
    fn->name = f->name;
    fn->count = (unsigned char)(code->line);

    Bobject* bin = malloc(sizeof(Bobject));
    bin->type = BFUNCTION;
    bin->value.bfun = fn;
    bin->refs = 0;

    add_to_stack(bstate->callStack[bstate->stackPos], f->name, bin);
    return bstate->none;
}

Bobject* bt_call_function(Bcode* code, bcon_State *bstate){
    opcall *c = code->value.call;
    Bobject *fun = c->callee->func(c->callee, bstate);

    if (bstate->islocked == BLOCK_ERRORED)
        return bstate->none;

    bargs *argv = c->argv;

    if (c->count) {
        int count = c->count;

        Bcode* args = c->args;
        Bobject **argv_list = argv->argv;

        for(int i = 0; i < count; ++i) {
            argv_list[i] = args->func(args, bstate);
            args = args->next;

            if (bstate->islocked == BLOCK_ERRORED)
                return bstate->none;
        }
    }

    Bobject* returns;

    switch (fun->type) {
        case BFUNCTION:
            bfunction *bf = fun->value.bfun;
            returns = bf->func(argv, bf, bstate);
            break;

        case BFUNCTION2:
            bfunction2 *bf2 = fun->value.bfun2;
            returns = bf2->func(argv, bf2, bstate);
            break;

        case BTYPE:
            btype *typ = fun->value.btype;
            returns = typ->func(argv, typ, bstate);
            break;

        default:
            returns = _type_error(bstate, my_concat(check_type(fun), " data is not callable.", 0), code->line);
            break;
    }

    return returns;
}

Bobject *bt_exec_function(bargs *args, bfunction *fn, bcon_State *bstate){
    if (args->count != fn->count)
        return _type_error(bstate, "fn() expects 2 arguments but was given 1.", args->line);

    ++(bstate->stackPos);

    if (bstate->stackPos == bstate->stackCapacity){
        --(bstate->stackPos);
        return _runtime_error(bstate, "a stackOverflow occured while calling a function");
    }

    Stack *callStack = create_callStack(7);
    bstate->callStack[bstate->stackPos] = callStack;

    if (fn->this)
        add_to_stack(callStack, "this", fn->this);

    int count = args->count;
    char **params = fn->params;

    Bobject **argv = args->argv;

    for (int i = 0; i < count; ++i)
        add_to_stack(callStack, params[i], argv[i]);

    block_evaluator_start(fn->code, bstate);

    delete_stack(callStack);
    --(bstate->stackPos);

    if (bstate->islocked == BLOCK_RETURNED) {
        bstate->islocked = BLOCK_ISRUNNING;
        return bstate->return_value;
    }

    return bstate->none;
}


