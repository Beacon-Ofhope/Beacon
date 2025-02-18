#ifndef H_BEACON_FS
#define H_BEACON_FS

#include <stdio.h>
#include "../../parser/Includes/bobject.h"


#define FS_FILE (FILE *)(((bfunction2*)(fun->d))->ptr)

Bobject *Open_Fs_bn(Bobject *args, Bobject *fun, bcon_State *bstate);

Bobject *Bn_Fs();

#endif