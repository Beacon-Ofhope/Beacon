#include <stdio.h>
#include <stdlib.h>
#include "bcode.h"
#include "bytec.h"
#include "bobject.h"
#include "eval.h"

Eval * evaluator_read(Inter * exec, Stack * memory){
	Eval* run = malloc(sizeof(Eval));
	run->tok = exec->start;
    run->file = exec->file;
    run->memory = memory;

	return run;
}

void evaluator_advance(Eval* run){
	if (run->tok != NULL)
		run->tok = run->tok->next;
}

void evaluator_start(Eval * run){
	while (run->tok != NULL) {
		run->tok->func(run->tok, run->memory);
        evaluator_advance(run);
	}
}
