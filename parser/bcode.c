#include "bcode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bytec.h"
#include "parser.h"
#include "../Include/helpers.h"


Inter* inter_read(const Parser* pls){
	Inter* exec = malloc(sizeof(Inter));
	exec->tok = pls->start;
	exec->file = pls->file;
    exec->start = NULL;
    exec->recent = NULL;

    return exec;
}

Inter* inter_read_within(AstNode* start, char* file){
	Inter* exec = malloc(sizeof(Inter));
	exec->tok = start;
	exec->file = file;
    exec->start = NULL;
    exec->recent = NULL;

    return exec;
}

Bcode* mk_safe_bcode() {
    Bcode *bin = malloc(sizeof(Bcode));
    bin->next = NULL;
    return bin;
}

void inter_advance(Inter* exec){
    AstNode* temp = NULL;

    if (exec->tok != NULL) {
	    temp = exec->tok->next;
        free(exec->tok);
        exec->tok = temp;
	}
}

void append_bcode(Inter * exec, Bcode* new_ast) {
    if (exec->start == NULL) {
        exec->start = new_ast;
        exec->recent = new_ast;
    } else {
        exec->recent->next = new_ast;
        exec->recent = new_ast;
    }
}

void append_block_bcode(Bcode **start, Bcode **recent, Bcode *new_data){
    if (*start == NULL){
        *start = new_data;
        *recent = new_data;
    } else {
        (*recent)->next = new_data;
        (*recent) = new_data;
    }
}

Bcode* i_call_function(AstNode* tok, Inter * pls){
    Bcode* bin = mk_safe_bcode();
    bin->type = I_CALL_FN;
    bin->func = bt_call_function;
    bin->value.str_value = copy_string_safely(tok->left->value.str_value);

    Inter* b_interpreter = inter_read_within(tok->right, pls->file);
    inter_interpret(b_interpreter);

    bin->left = b_interpreter->start;
    return bin;
}

Bcode* i_mk_list(AstNode* tok, Inter* pls){
    Bcode* bin = mk_safe_bcode();
    bin->type = I_LIST;
    bin->func = bt_list;

    Inter *b_interpreter = inter_read_within(tok->left, pls->file);
    inter_interpret(b_interpreter);

    bin->left = b_interpreter->start;
    return bin;
}

Bcode* i_eval_ast(AstNode* value, Inter * pls){
    if (value->type == P_FN_CALL) {
        return i_call_function(value, pls);

    } else if (value->type == P_LIST){
        return i_mk_list(value, pls);

    }

    Bcode* bin = mk_safe_bcode();

    if (value->type == P_NUM) {
        bin->value.num_value = value->value.num_value;
        bin->type = I_NUM;
        bin->func = bt_num;

    } else if (value->type == P_STR) {
        bin->value.str_value = copy_string_safely(value->value.str_value);
        bin->type = I_STR;
        bin->func = bt_str;

    } else if (value->type == P_VAR) {
        bin->value.str_value = copy_string_safely(value->value.str_value);
        bin->type = I_VAR;
        bin->func = bt_get_variable;

	} else if (value->type == P_BIN_OP) {
		bin->left = i_eval_ast(value->left, pls);
		bin->right = i_eval_ast(value->right, pls);

		if (strcmp(value->value.str_value, "+") == 0)
			bin->func = bt_add_num;
		else if (strcmp(value->value.str_value, "-") == 0)
			bin->func = bt_minus_num;
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
    return bin;
}


//Dcode* make_function(Inter* pls){
//    Dcode* bin = (Dcode*)malloc(sizeof(Dcode));
//    bin->type = I_FN;
//    bin->func = bt_make_function;
//    bin->value.str_value = pls->tok->value.str_value;
//
//    Dcode* dcode_parameters_head = NULL;
//    Interpo_start(Interpo_read(&pls->tok->left), &dcode_parameters_head);
//    bin->left = dcode_parameters_head;
//
//    Dcode* dcode_block_head = NULL;
//    Interpo_start(Interpo_read(&pls->tok->right), &dcode_block_head);
//
//    bin->right = dcode_block_head;
//    return bin;
//}


Bcode* i_make_variable(Inter* pls) {
    Bcode* bin = mk_safe_bcode();
    bin->type = I_VAR_MAKE;
    bin->func = bt_make_variable;
    bin->value.str_value = copy_string_safely(pls->tok->value.str_value);
    bin->left = i_eval_ast(pls->tok->left, pls);

    return bin;
}

// interpreting code block to bytecode
Bcode *i_code_to_bytecode(Inter *pls, AstNode *stats){
    Parser *par = calloc(1, sizeof(Parser));
    par->file = pls->file;
    par->start = stats->right;

    Inter *b_interpreter = inter_read(par);
    inter_interpret(b_interpreter);

    return b_interpreter->start;
}

Bcode *i_make_if_stat(Inter* pls){
    Bcode *start = NULL;
    Bcode *recent = NULL;

    AstNode *stats = pls->tok->left;
    AstNode *temp_stats = NULL;

    while(stats != NULL){
        Bcode *data = mk_safe_bcode();
        data->left = i_eval_ast(stats->left, pls);
        data->right = i_code_to_bytecode(pls, stats);

        append_block_bcode(&start, &recent, data);
        temp_stats = stats->next;
        free(stats);
        stats = temp_stats;
    }

    return start;
}

Bcode *i_make_if(Inter *pls) {
    Bcode *bin = mk_safe_bcode();
    bin->type = I_IF;
    bin->func = bt_if;

    bin->left = i_make_if_stat(pls);
    bin->type = I_IF;
    
    return bin;
}

void inter_interpret(Inter* pls) {
    while (pls->tok != NULL) {
        if (pls->tok->type == P_VAR_ASSIGN){
            append_bcode(pls, i_make_variable(pls));

        } else if (pls->tok->type == P_IF) {
            append_bcode(pls, i_make_if(pls));

        } else {
            append_bcode(pls, i_eval_ast(pls->tok, pls));

        }

        inter_advance(pls);
    }
}
