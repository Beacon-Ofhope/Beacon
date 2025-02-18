#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/types/siginfo_t.h>

#include "../Includes/bcode.h"
#include "../Includes/bobject.h"
#include "../Includes/bytecode.h"


Bobject* mk_safe_Bobject(){
    Bobject* obj = malloc(sizeof(Bobject));
    obj->refs = 0;
    return obj;
}

// executing code blocks and statements
void block_evaluator_start(Bcode *run, bcon_State *bstate){
    Bcode *start = run;
    while (start != NULL && bstate->islocked == BLOCK_ISRUNNING) {
        start->func(start, bstate);
        start = start->next;
    }
}

void append_param(Param** start, Param** recent, Param* new_token) {
	if (*start == NULL) {
	    *start = new_token;
        *recent = new_token;
    } else {
        (*recent)->next = new_token;
        *recent = new_token;
    }
}

