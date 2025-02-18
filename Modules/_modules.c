#include <stdlib.h>

#include "../parser/Includes/bobject.h"

Bobject *mk_module_fun(char *name, Bobject *(*fun)(bargs *, bfunction2 *, bcon_State *), void *ptr)
{
    bfunction2 *fn = malloc(sizeof(bfunction2));
    fn->func = fun;
    fn->ptr = ptr;
    fn->name = name;

    Bobject *bin = malloc(sizeof(Bobject));
    bin->type = BFUNCTION2;
    bin->value.bfun2 = fn;
    bin->refs = 0;

    return bin;
}

Bobject *mk_number(double number) {
    Bobject *num = malloc(sizeof(Bobject));
    num->value.num_value = number;
    num->type = BNUMBER;
    num->refs = 0;

    return num;
}

Bobject *mk_string(char *str) {
    Bobject *st = malloc(sizeof(Bobject));
    st->value.str_value = str;
    st->type = BSTRING;
    st->refs = 0;

    return st;
}

Bobject *mk_mod_number(double x){
    Bobject *n = malloc(sizeof(Bobject));
    n->value.num_value = x;
    n->type = BNUMBER;
    n->refs = 0;
    n->d = NULL;

    return n;
}

Bobject *mk_mod_string(char *s){
    Bobject *n = malloc(sizeof(Bobject));
    n->value.str_value = s;
    n->type = BSTRING;
    n->refs = 0;
    n->d = NULL;

    return n;
}

Bobject *mk_mod_interface(char *name, Stack *attrs, unsigned char count, bcon_State *bstate)
{
    binterface *in_face = malloc(sizeof(binterface));
    in_face->attrs = attrs;
    in_face->count = count;

    Bobject *bin = mk_Bobject(bstate);
    bin->value.str_value = name;
    bin->type = BINTERFACE;
    bin->d = in_face;

    return bin;
}

Bobject *mk_mod_list(size_t count, Bobject **members, bcon_State *bstate)
{
    blist *list = malloc(sizeof(blist));
    list->values = members;
    list->capacity = count;
    list->count = count;

    Bobject *bin = mk_Bobject(bstate);
    bin->type = BLIST;
    bin->d = list;
    bin->value.str_value = "list";

    return bin;
}
