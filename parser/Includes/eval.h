
#ifndef H_EVALUATOR
#define H_EVALUATOR

#include "parser.h"
#include "bcode.h"
#include "bobject.h"


typedef struct EVAL_UP{
	Bcode *tok;
	Bcode *start;
	char *file;
} Eval;


Eval *evaluator_read(Inter *exec);

void evaluator_start(Eval *pls, bcon_State *bstate);


#endif
