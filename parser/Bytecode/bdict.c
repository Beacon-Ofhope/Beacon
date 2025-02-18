#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Includes/bcode.h"
#include "../Includes/bobject.h"
#include "../Includes/bytecode.h"

Bobject *bt_dict(Bcode *code, bcon_State *bstate){
    opdict *dt = code->value.dict;
    Stack *dictionary = create_stack(((code->line) ? (0 - code->line) : -1));

    char **keys = dt->keys;
    Bcode *values = dt->values;

    // keys and val number
    int count = code->line;
    Bobject *exec_val;

    for (int i = 0; i < count; ++i){
        exec_val = values->func(values, bstate);

        if (bstate->islocked != BLOCK_ISRUNNING)
            return bstate->none;
        
        add_to_stack(dictionary, keys[i], exec_val);
        values = values->next;
    }
    
    Bobject *dict_obj = mk_Bobject(bstate);
    dict_obj->value.attrs = dictionary;
    dict_obj->type = BDICT;
    dict_obj->refs = 0;

    return dict_obj;
}
