#ifndef H_TIME_LIB
#define H_TIME_LIB

#include <time.h>
#include "../../parser/Includes/bobject.h"

struct tm mk_tm_struct(binterface *arg, bcon_State *bstate);

Bobject *mk_bn_tm_struct(struct tm *when, bcon_State *bstate);

Bobject *Bn_Time();

#endif