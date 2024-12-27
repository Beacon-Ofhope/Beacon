#ifndef _API_H
#define _API_H

#include "../parser/Includes/bobject.h"

Bobject *print(Bobject *args, Bobject *fun, bcon_State *bstate);

Bobject *input(Bobject *args, Bobject *fun, bcon_State *bstate);

#endif