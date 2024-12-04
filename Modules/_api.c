#include <stdio.h>
#include <stdlib.h>

#include "../parser/dobject.h"



Dobject* print(Dobject *args, Stack *stack){
	Dobject* current = args;

	while (current != NULL){
		if (current->type == D_NUM)
			printf("%.0lf ", current->value.num_value);
		else
			printf("%s ", current->value.str_value);
		
		current = current->next;
	}
	printf("\n");
	return b_None();
}
