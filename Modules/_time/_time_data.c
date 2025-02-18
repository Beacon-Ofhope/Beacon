#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include "../Includes/_time.h"
#include "../Includes/_modules.h"
#include "../../parser/Includes/bobject.h"
#include "../../parser/Includes/bytecode.h"


Bobject* mk_bn_tm_struct(struct tm* when, bcon_State* bstate){
	Stack *attr = create_stack(9);

	add_to_stack_i(attr, "tm_year", mk_number(when->tm_year, bstate));
	add_to_stack_i(attr, "tm_mon", mk_number(when->tm_mon, bstate));
	add_to_stack_i(attr, "tm_mday", mk_number(when->tm_mday, bstate));
	add_to_stack_i(attr, "tm_hour", mk_number(when->tm_hour, bstate));
	add_to_stack_i(attr, "tm_min", mk_number(when->tm_min, bstate));
	add_to_stack_i(attr, "tm_sec", mk_number(when->tm_sec, bstate));
	add_to_stack_i(attr, "tm_wday", mk_number(when->tm_wday, bstate));
	add_to_stack_i(attr, "tm_yday", mk_number(when->tm_yday, bstate));
	add_to_stack_i(attr, "tm_isdst", mk_number(when->tm_isdst, bstate));

	// has_tm_zone(attr, loc, bstate);
	return mk_mod_interface("[Interface struct_time]", attr, 9, bstate);
}

struct tm mk_tm_struct(binterface* arg, bcon_State* bstate){
    Stack* _set = arg->attrs;
    Node **members = _set->table;

    char* key;
	Node *n_mem;
	struct tm when;

	for (size_t i = 0; i < arg->count; i++){
		n_mem = members[i];
		if (n_mem != NULL) {
			key = n_mem->key;

			if (strcmp(key, "tm_year") == 0)
				when.tm_year = get_from_stack(_set, key)->value.num_value;

			else if (strcmp(key, "tm_mon") == 0)
				when.tm_mon = get_from_stack(_set, key)->value.num_value;

			else if (strcmp(key, "tm_wday") == 0)
				when.tm_wday = get_from_stack(_set, key)->value.num_value;

			else if (strcmp(key, "tm_mday") == 0)
				when.tm_mday = get_from_stack(_set, key)->value.num_value;

			else if (strcmp(key, "tm_hour") == 0)
				when.tm_hour = get_from_stack(_set, key)->value.num_value;

			else if (strcmp(key, "tm_min") == 0)
				when.tm_min = get_from_stack(_set, key)->value.num_value;

			else if (strcmp(key, "tm_sec") == 0)
				when.tm_sec = get_from_stack(_set, key)->value.num_value;

			else if (strcmp(key, "tm_isdst") == 0)
				when.tm_isdst = get_from_stack(_set, key)->value.num_value;
		}
	}
    
    return when;
}

