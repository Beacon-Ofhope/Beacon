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
	while (run->tok != NULL) {
		run->tok->func(run->tok, bstate);
		run->tok = run->tok->next;
	}
}
