#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../parser/bobject.h"


Bobject* print(Bobject* args, Stack* stack){
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
