#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "Includes/_imports.h"
#include "Includes/_string.h"
#include "Includes/_modules.h"
#include "../parser/Includes/bobject.h"
#include "../parser/Includes/bytecode.h"


Bobject* concat_string_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    if (!args || args->value.num_value < 2)
        return _type_error(bstate, "'String.concat()' expects many args (...)");

    Bobject** arg = BARG;
    char* res = arg[0]->value.str_value;

    for (int i = 1; i < args->value.num_value; i++)
        res = my_concat(res, arg[i]->value.str_value, 0);

    return mk_string(res, bstate);
}

Bobject *uppercase_string_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    if (args == NULL){
        return args;
    }

    char* up = malloc(strlen(_BSTRING)+1);

    for (long i = 0; i < strlen(_BSTRING); i++){
        up[i] = toupper(_BSTRING[i]);
    }

    up[strlen(_BSTRING)] = '\0';
    return mk_string(up, bstate);
}

Bobject *lowercase_string_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    if (args == NULL){
        return args;
    }

    char* low = malloc(strlen(_BSTRING)+1);

    for (long i = 0; i < strlen(_BSTRING); i++){
        low[i] = tolower(_BSTRING[i]);
    }

    low[strlen(_BSTRING)] = '\0';
    return mk_string(low, bstate);
}

Bobject *Bn_String() {
    Stack *M = create_stack(4);

    add_to_stack(M, "concat", mk_module_fun("[Function String.concat]", concat_string_bn, NULL));
    add_to_stack(M, "uppercase", mk_module_fun("[Function String.uppercase]", uppercase_string_bn, NULL));
    add_to_stack(M, "lowercase", mk_module_fun("[Function String.lowercase]", lowercase_string_bn, NULL));

    bmodule *mod = malloc(sizeof(bmodule));
    mod->attrs = M;

    Bobject *bin = mk_safe_Bobject();
    bin->value.str_value = "[Module String]";
    bin->type = BMODULE;
    bin->d = mod;

    return bin;
}