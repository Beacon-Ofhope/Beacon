#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../Includes/_fs.h"
#include "../Includes/_modules.h"
#include "../../parser/Includes/bytecode.h"
#include "../../parser/Includes/bobject.h"

Bobject *close_Open_fs_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    if (fclose(FS_FILE) == EOF)
        return _sys_error(bstate, errno);

    return bstate->none;
}

Bobject *write_Open_fs_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    if (!args)
        return _type_error(bstate, "'fs.Open.write()' takes 1 argument (data).");

    if (fputs(BARG[0]->value.str_value, FS_FILE) == EOF)
        return _sys_error(bstate, errno);

    return bstate->none;
}

Bobject *flush_Open_fs_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    if (fflush(FS_FILE))
        return _sys_error(bstate, errno);

    return bstate->none;
}

Bobject *read_Open_fs_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    size_t bufSize;

    if (args)
        bufSize = (size_t)(BARG[0]->value.num_value);
    else
        bufSize = 2048;

    char *buffer = malloc(bufSize+1);
    size_t ret = fread(buffer, 1, bufSize, FS_FILE);

    if (strlen(buffer) == 0)
        return bstate->none;

    buffer[bufSize] = '\0';
    return mk_string(buffer, bstate);
}

Bobject *Open_Fs_bn(Bobject *args, Bobject *fun, bcon_State *bstate) {
    if (!args || args->value.num_value < 2)
        return _type_error(bstate, "'fs.Open()' requires arguments (fileName, mode)");

    Bobject** arg = BARG;

    FILE *_file = malloc(sizeof(FILE));
    _file = fopen(arg[0]->value.str_value, arg[1]->value.str_value);

    if (_file == NULL)
        return _sys_error(bstate, errno);

    Stack *M = create_stack(6);

    add_to_stack_i(M, "name", mk_string(arg[0]->value.str_value, bstate));
    add_to_stack_i(M, "mode", mk_string(arg[1]->value.str_value, bstate));

    add_to_stack_i(M, "close", mk_module_fun("[Method: Open.close]", close_Open_fs_bn, (void *)_file));
    add_to_stack_i(M, "flush", mk_module_fun("[Method: Open.flush]", flush_Open_fs_bn, (void *)_file));
    add_to_stack_i(M, "read", mk_module_fun("[Method: Open.read]", read_Open_fs_bn, (void *)_file));
    add_to_stack_i(M, "write", mk_module_fun("[Method: Open.write]", write_Open_fs_bn, (void *)_file));

    bclass *mod = malloc(sizeof(bclass));
    mod->attrs = M;

    Bobject *bin = malloc(sizeof(Bobject));
    bin->value.str_value = "[Object fileStream]";
    bin->type = BOBJECT;
    bin->d = mod;

    return bin;
}

