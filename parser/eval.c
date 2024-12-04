#include <stdio.h>
#include <stdlib.h>
#include "dcode.h"
#include "bytec.h"
#include "dobject.h"
#include "eval.h"

Eval* evaluator_read(Dcode** toks){
	Eval* pls = (struct EVAL_UP*)malloc(sizeof(struct EVAL_UP));
	pls->tok = *toks;

	return pls;
}

void evaluator_advance(Eval* pls){
	if (pls->tok != NULL)
		pls->tok = pls->tok->next;
}

void appendData(Dcode** head, Dcode* newToken) {
    if (*head == NULL) {
        *head = newToken;
    } else {
        Dcode* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newToken;
    }
}

void evaluator_start(Eval* pls, Stack* stack){

	while (pls->tok != NULL) {
		pls->tok->func(pls->tok, stack);
        evaluator_advance(pls);
    }
}
