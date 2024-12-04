#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "dcode.h"
#include "bytec.h"
#include "dobject.h"


Inter* Interpo_read(AstNode** toks){
	Inter* pls = (struct DCODE_UP*)malloc(sizeof(struct DCODE_UP));
	pls->tok = *toks;
}

void Interpo_advance(Inter* pls){
	if (pls->tok != NULL)
		pls->tok = pls->tok->next;
}

void appendDcode(Dcode** head, Dcode* newToken) {
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

Dcode* eval_ast(AstNode* value){
    Dcode* bin = malloc(sizeof(Dcode));

    if (value->type == P_NUM){
        bin->value.num_value = value->value.num_value;
        bin->type = I_NUM;
        bin->func = bt_num;

    } else if (value->type == P_STR){
        bin->value.str_value = value->value.str_value;
        bin->type = I_STR;
        bin->func = bt_str;

    } else if (value->type == P_VAR){
        bin->value.str_value = value->value.str_value;
        bin->type = I_VAR;
        bin->func = bt_get_variable;

	} else if (value->type == P_BIN_OP) {
		bin->left = eval_ast(value->left);
		bin->right = eval_ast(value->right);

		if (strcmp(value->value.str_value, "+") == 0)
			bin->func = bt_add_num;
		else if (strcmp(value->value.str_value, "-") == 0)
			bin->func = bt_sub_num;
		else if (strcmp(value->value.str_value, "/") == 0)
			bin->func = bt_div_num;
		else if (strcmp(value->value.str_value, "*") == 0)
			bin->func = bt_mult_num;
        else if (strcmp(value->value.str_value, "<") == 0)
            bin->func = bt_mult_num;
        else if (strcmp(value->value.str_value, ">") == 0)
            bin->func = bt_mult_num;
        else if (strcmp(value->value.str_value, "!") == 0)
            bin->func = bt_mult_num;
        else if (strcmp(value->value.str_value, "<=") == 0)
            bin->func = bt_mult_num;
        else if (strcmp(value->value.str_value, ">=") == 0)
            bin->func = bt_mult_num;
        else if (strcmp(value->value.str_value, "!=") == 0)
            bin->func = bt_mult_num;
	}
    else if (value->type == P_FN_CALL) {
        bin = call_function(value);
    }

    return bin;
}

Dcode* make_variable(Inter* pls){
    Dcode* bin = malloc(sizeof(Dcode));
    bin->type = I_VAR_MAKE;
    bin->func = bt_make_variable;
    bin->value.str_value = pls->tok->value.str_value;
    bin->left = eval_ast(pls->tok->left);
    return bin;
}

Dcode* call_function(AstNode* tok){
    Dcode* bin = (Dcode*)malloc(sizeof(Dcode));
    bin->type = I_CALL_FN;
    bin->func = bt_call_function;
    bin->value.str_value = tok->left->value.str_value;

    Dcode* dcode_args_head = NULL;
    Interpo_start(Interpo_read(&(tok->right)), &dcode_args_head);

    Dcode* codec = NULL;
    codec = dcode_args_head;

    printf("....\n");

    while (codec != NULL){
        printf("%p***\n", codec);
        codec = codec->next;
    }

    bin->left = dcode_args_head;
    return bin;
}


Dcode* make_function(Inter* pls){
    Dcode* bin = (Dcode*)malloc(sizeof(Dcode));
    bin->type = I_FN;
    bin->func = bt_make_function;
    bin->value.str_value = pls->tok->value.str_value;

    Dcode* dcode_parameters_head = NULL;
    Interpo_start(Interpo_read(&pls->tok->left), &dcode_parameters_head);
    bin->left = dcode_parameters_head;

    Dcode* dcode_block_head = NULL;
    Interpo_start(Interpo_read(&pls->tok->right), &dcode_block_head);

    bin->right = dcode_block_head;
    return bin;
}

void Interpo_start(Inter* pls, Dcode** head){
	while (pls->tok != NULL) {
        
        if (pls->tok->type == P_VAR_ASSIGN){
        	appendDcode(head, make_variable(pls));
        }
        else if (pls->tok->type == P_FN_CALL){
			appendDcode(head, call_function(pls->tok));
		}
        else if (pls->tok->type == P_FN){
            appendDcode(head, make_function(pls));
        }
        else {
            appendDcode(head, eval_ast(pls->tok));
        }
        Interpo_advance(pls);
    }
}
