#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../parser/Includes/bobject.h"
#include "../parser/Includes/bytecode.h"

#include "Includes/_string.h"

Bobject *make_string(char *value){
    Bobject *str = calloc(sizeof(Bobject), 1);
    str->value.str_value = value;
    str->type = B_STR;

    return str;
}

Bobject *concat_string(Bobject *args, Bobject *fun, bcon_State *bstate) {
    Bobject *arg = args;
    char *value = "";

    while (args != NULL){
        strcat(value, _BSTRING);
        args = args->next;
    }

    printf("%s..\n", value);

    return make_string(value);
}

Bobject *uppercase_string(Bobject *args, Bobject *fun, bcon_State *bstate) {
    if (args == NULL){
        return args;
    }

    char* up = malloc(strlen(_BSTRING)+1);

    for (long i = 0; i < strlen(_BSTRING); i++){
        up[i] = toupper(_BSTRING[i]);
    }

    up[strlen(_BSTRING)] = '\0';
    return make_string(up);
}

Bobject *lowercase_string(Bobject *args, Bobject *fun, bcon_State *bstate) {
    if (args == NULL){
        return args;
    }

    char* low = malloc(strlen(_BSTRING)+1);

    for (long i = 0; i < strlen(_BSTRING); i++){
        low[i] = tolower(_BSTRING[i]);
    }

    low[strlen(_BSTRING)] = '\0';
    return make_string(low);
}


Bobject *M_String() {
    Bobject *module = calloc(sizeof(Bobject), 1);

    Stack *M = create_stack();
    add_to_stack(M, "concat", bt_make_b_fun(concat_string));
    add_to_stack(M, "up_case", bt_make_b_fun(uppercase_string));
    add_to_stack(M, "low_case", bt_make_b_fun(lowercase_string));

    module->attrs = M;
    return module;
}