
#ifndef H_EVALUATOR
#define H_EVALUATOR

#include "parser.h"
#include "dcode.h"
#include "dobject.h"


typedef struct EVAL_UP{
	Dcode* tok;
} Eval;


Eval* evaluator_read(Dcode** toks);

void evaluator_start(Eval* pls, Stack* stack);


#endif
