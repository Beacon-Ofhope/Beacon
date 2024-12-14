#include <stdlib.h>
#include <string.h>
#include <stdio.h>


char* copy_string_safely(char * src) {
    char* dest = malloc(strlen(src)+1);
    strcpy(dest, src);
    free(src);
    return dest;
}

