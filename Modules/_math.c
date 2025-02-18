#include <math.h>
#include <stdlib.h>

#include "Includes/_modules.h"
#include "../parser/Includes/bytecode.h"
#include "../parser/Includes/bobject.h"


Bobject *acos_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    return mk_number(acos(args->value.num_value), bstate);
}

Bobject *asin_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    return mk_number(acos(args->value.num_value), bstate);
}

Bobject *atan_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    return mk_number(atan(args->value.num_value), bstate);
}

Bobject *Bn_Math() {
    Stack *M = create_stack(4);

    add_to_stack(M, "acos", mk_module_fun("[Function Math.acos]", acos_bn, NULL));
    add_to_stack(M, "asin", mk_module_fun("[Function Math.asin]", asin_bn, NULL));
    add_to_stack(M, "atan", mk_module_fun("[Function Math.atan]", atan_bn, NULL));

    bmodule *mod = malloc(sizeof(bmodule));
    mod->attrs = M;

    Bobject *bin = mk_safe_Bobject();
    bin->value.str_value = "[Module Math]";
    bin->type = BMODULE;
    bin->d = mod;

    return bin;
}

