#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../parser/Includes/bobject.h"

Bobject* print(Bobject* args, Bobject* fun, bcon_State *bstate){
	Bobject* arg = args;

	while (arg != NULL){
		if (arg->type == B_NUM)
			printf("%.0lf ", arg->value.num_value);
		else
			printf("%s ", arg->value.str_value);

		arg = arg->next;
	}
	printf("\r\n");
	return b_None();
}

Bobject* input(Bobject* args, Bobject* fun, bcon_State *bstate){
	Bobject* arg = args;
	while (arg != NULL){
		printf("%s", arg->value.str_value);
		arg = arg->next;
	}

	char *input_data;
	scanf("%s", input_data);

	char *input_str = malloc(strlen(input_data));
	strcpy(input_str, input_data);

	return BC_STRING(input_str);
}
