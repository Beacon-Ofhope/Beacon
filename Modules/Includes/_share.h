#ifndef H_SHARE_LIB
#define H_SHARE_LIB

#include "../../parser/Includes/bobject.h"

void print_string_share(Bobject *arg);

void print_number_share(Bobject *arg);

void print_data_share(Bobject *arg);

void print_list_share(Bobject *arg);

void print_dictionary_share(Bobject* arg);

Bobject *Bn_Share();

#endif