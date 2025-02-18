#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

char* my_concat(char* a, const char* b, int wipe){
    char* c = malloc(strlen(a)+strlen(b)+1);

    size_t i = strlen(a);
    memcpy(c, a, i);

    size_t j = 0;
    for(; i < (strlen(b)+strlen(a)); i++){
        c[i] = b[j];
        j++;
    }
    c[i] = '\0';

    if (wipe)
        free(a);

    return c;
}

char* mk_module_name(char* name){
    char *_module = my_concat("[Module '", name, 0);
    return my_concat(_module, "']", 1);
}

char *mk_relative_path(char* name){
    char *ret = realpath(name, NULL);

    if (ret == NULL)
        return NULL;

    return strdup(ret);
}

char* mk_path_name(char* name, char* base){
    char *final = my_concat(base, name, 1);

    if (access(final, F_OK)){
        free(final);
        return NULL;
    }

    char* usable_path = mk_relative_path(final);
    free(final);

    return usable_path;
}

char* mk_path_name_from_beacon_modules(char* name, char* base){
    char *final = my_concat(base, "beacon_modules/", 0);
    final = my_concat(final, name, 1);

    if (access(final, F_OK)){
        free(final);
        return NULL;
    }

    char* usable_path = mk_relative_path(final);
    free(final);

    return usable_path;
}

char* mk_path_name_here(char* name){
    if (access(name, F_OK))
        return NULL;

    return mk_relative_path(name);
}

