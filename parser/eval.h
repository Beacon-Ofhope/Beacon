
#ifndef H_EVALUATOR
#define H_EVALUATOR

#include "parser.h"
#include "bcode.h"
#include "bobject.h"


typedef struct EVAL_UP{
	Bcode * tok;
	char * file;
	Stack * memory;
} Eval;


Eval * evaluator_read(Inter * exec, Stack * memory);

void evaluator_start(Eval * pls);


#endif
