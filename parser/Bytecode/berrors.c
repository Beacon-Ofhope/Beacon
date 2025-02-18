#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../Includes/bcode.h"
#include "../Includes/bobject.h"
#include "../Includes/bytecode.h"
#include "../../Modules/Includes/_imports.h"

Bobject* _attribute_error(bcon_State *bstate, Bobject *data, unsigned int line){
    bstate->islocked = BLOCK_ERRORED;
    
    bstate->return_value = mk_error("AttributeError", my_concat("can't get attribute from a ", check_type(data), 0), bstate->file, line, -808);

    bstate->none;
}

Bobject* _no_attribute_error(bcon_State *bstate, Bobject *data, char* attr_name, unsigned int line){
    bstate->islocked = BLOCK_ERRORED;

    bstate->return_value = mk_error("AttributeError", my_concat(my_concat(check_type(data), " object has no attribute ", 0), attr_name, 1), bstate->file, line, -808);

    return bstate->none;
}

Bobject* _set_attribute_error(bcon_State *bstate, Bobject *data, unsigned int line){
    bstate->islocked = BLOCK_ERRORED;
    
    bstate->return_value = mk_error("AttributeError", my_concat("you can't set attribute to a ", check_type(data), 0), bstate->file, line, -808);

    return bstate->none;
}

Bobject* _reference_error(bcon_State *bstate, char *name, unsigned int line){
    bstate->islocked = BLOCK_ERRORED;
    bstate->return_value = mk_error("ReferenceError", my_concat(name, " is not defined.", 0), bstate->file, line, -808);

    return bstate->none;
}

Bobject* _sys_error(bcon_State *bstate, int error_no){
    bstate->islocked = BLOCK_ERRORED;
    bstate->return_value = mk_error("Error", NULL, bstate->file, 0, error_no);

    return bstate->none;
}

Bobject* _type_error(bcon_State *bstate, char *message, unsigned int line){
    bstate->islocked = BLOCK_ERRORED;
    bstate->return_value = mk_error("TypeError", message, bstate->file, line, BERRNO);
    return bstate->none;
}

Bobject* _runtime_error(bcon_State *bstate, char *message){
    bstate->islocked = BLOCK_ERRORED;
    bstate->return_value = mk_error("RuntimeError", message, bstate->file, 0, BERRNO);
    return bstate->none;
}


Bobject *_import_error(bcon_State *bstate, char *message){
    bstate->islocked = BLOCK_ERRORED;
    bstate->return_value = mk_error("ImportError", my_concat(my_concat("Can't import module '", message, 0), "'.", 1), bstate->file, 0, BERRNO);
    return bstate->none;
}

Bobject* _import_error2(bcon_State *bstate, char *message, unsigned int line){
    bstate->islocked = BLOCK_ERRORED;
    bstate->return_value = mk_error("ImportError", message, bstate->file, line, BERRNO);
    return bstate->none;
}

char *check_type(Bobject *data){
    switch (data->type) {
        case BSTRING:
            return "String";
        case BNUMBER:
            return "Number";
        case BNONE:
            return "None";
        case BLIST:
            return "List";
        case BDICT:
            return "Dict";
        case BMODULE:
            return "Module";
        case BFUNCTION:
        case BFUNCTION2:
            return "Function";
        case BTYPE:
            return get_from_stack(((btype*)(data->d))->attrs, "__name__")->value.str_value;
        case BOBJECT:
            return data->value.str_value;
        default:
            break;
    }
    return "unknown";
}
