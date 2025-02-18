#include <stdio.h>
#include <stdlib.h>

#include "Includes/bytecode.h"
#include "Includes/bobject.h"
#include "Includes/bcode.h"
#include "Includes/eval.h"

Eval * evaluator_read(Inter *exec){
	Eval* run = calloc(sizeof(Eval), 1);
	run->start = exec->start;
	run->tok = exec->start;
    run->file = exec->file;
	return run;
}

void evaluator_start(Eval *run, bcon_State *bstate){
	Bcode *tok = run->tok;

	while (tok != NULL && bstate->islocked == BLOCK_ISRUNNING) {
		tok->func(tok, bstate);
		tok = tok->next;
	}
}
