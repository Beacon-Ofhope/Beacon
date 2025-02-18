#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#include "../Includes/_modules.h"
#include "../Includes/_fs.h"
#include "../../parser/Includes/bytecode.h"
#include "../../parser/Includes/bobject.h"


Bobject *exists_fs_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    if (args && (access(BARG[0]->value.str_value, F_OK) == 0))
        return mk_number(1, bstate);

    return mk_number(0, bstate);
}

Bobject *getcwd_fs_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    size_t size = 100;
    char *buffer;

    while (1) {
        buffer = (char *)malloc(size);

        if (buffer== NULL){
            bstate->islocked = BLOCK_ERRORED;
            return NULL;
        }

        if (getcwd(buffer, size) == buffer)
            break;

        free(buffer);

        if (errno != ERANGE) {
            bstate->islocked = BLOCK_ERRORED;
            return bstate->none;
        }
        size += 100;
    }
    return mk_string(buffer, bstate);
}

Bobject *remove_fs_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    if (!args)
        return _type_error(bstate, "'fs.remove()' required 1 argument (pathName).");

    if (remove(BARG[0]->value.str_value) == -1)
        return _sys_error(bstate, errno);

    return bstate->none;
}

Bobject *rename_fs_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    if (args && args->value.num_value > 1){
        Bobject** arg = BARG;

        if (rename(arg[0]->value.str_value, arg[1]->value.str_value) == -1)
            return _sys_error(bstate, errno);
    } else {
        return _type_error(bstate, "'fs.rename()' requires args (old_path, new_path)");
    }

    return bstate->none;
}

Bobject *Bn_Fs() {
    Stack *M = create_stack(6);

    add_to_stack_i(M, "exists", mk_module_fun("[Function: fs.exists]", exists_fs_bn, NULL));
    add_to_stack_i(M, "getcwd", mk_module_fun("[Function: fs.getcwd]", getcwd_fs_bn, NULL));
    add_to_stack_i(M, "Open", mk_module_fun("[Function: fs.Open]", Open_Fs_bn, NULL));
    add_to_stack_i(M, "remove", mk_module_fun("[Function: fs.remove]", remove_fs_bn, NULL));
    add_to_stack_i(M, "rename", mk_module_fun("[Function: fs.rename]", rename_fs_bn, NULL));

    bmodule *mod = malloc(sizeof(bmodule));
    mod->attrs = M;

    Bobject *bin = malloc(sizeof(Bobject));
    bin->value.str_value = "[Module fs]";
    bin->type = BMODULE;
    bin->d = mod;

    return bin;
}

