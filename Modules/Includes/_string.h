#ifndef H_BEACON_STRING
#define H_BEACON_STRING

#include "../../parser/Includes/bobject.h"

#define _BSTRING    args->value.str_value

Bobject *M_String();

Bobject *concat_string(Bobject * args, Bobject * fun, bcon_State *bstate);

#endif
