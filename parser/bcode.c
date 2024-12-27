#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Includes/bcode.h"
#include "Includes/bytecode.h"
#include "Includes/parser.h"


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
    Bcode *bin = calloc(sizeof(Bcode), 1);
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
    bin->type = OP_FUNCTION_CALL;
    bin->func = bt_call_function;
    bin->left = i_eval_ast(tok->left, pls);
    bin->line = tok->line;

    Inter* b_interpreter = inter_read_within(tok->right, pls->file);
    inter_interpret(b_interpreter);

    bin->right = b_interpreter->start;
    return bin;
}

Bcode* i_mk_list(AstNode* tok, Inter* pls){
    Bcode* bin = mk_safe_bcode();
    bin->type = OP_LIST;
    bin->func = bt_list;

    Inter *b_interpreter = inter_read_within(tok->left, pls->file);
    inter_interpret(b_interpreter);

    bin->left = b_interpreter->start;
    return bin;
}

Bcode* i_get_attribute(AstNode* tok, Inter * pls){
    Bcode* bin = mk_safe_bcode();
    bin->type = OP_GET_ATTRIBUTE;
    bin->func = bt_get_attribute;
    bin->value.str_value = tok->value.str_value;
    bin->left = i_eval_ast(tok->left, pls);
    return bin;
}

Bcode* i_set_attribute(AstNode* tok, Inter * pls){
    Bcode* bin = mk_safe_bcode();
    bin->type = OP_SET_ATTRIBUTE;
    bin->func = bt_set_attribute;
    bin->value.str_value = tok->value.str_value;
    bin->left = i_eval_ast(tok->left, pls);
    bin->right = i_eval_ast(tok->right, pls);
    
    return bin;
}

Bcode* i_break(){
    Bcode *bin = mk_safe_bcode();
    bin->type = OP_BREAK;
    bin->func = bt_break;

    return bin;
}

Bcode* i_continue(){
    Bcode *bin = mk_safe_bcode();
    bin->type = OP_CONTINUE;
    bin->func = bt_continue;

    return bin;
}

Bcode* i_return(AstNode *tok, Inter *pls){
    Bcode *bin = mk_safe_bcode();
    bin->type = OP_RETURN;
    bin->left = i_eval_ast(tok->left, pls);
    bin->func = bt_return;

    return bin;
}

Bcode* i_not(AstNode *tok, Inter *pls){
    Bcode *bin = mk_safe_bcode();
    bin->left = i_eval_ast(tok->left, pls);
    bin->func = bt_not;
    bin->type = OP_NOT;

    return bin;
}

Bcode* i_eval_ast(AstNode* value, Inter * pls){
    switch (value->type){
        case P_FN_CALL:
            return i_call_function(value, pls);
        case P_LIST:
            return i_mk_list(value, pls);
        case P_ATTR:
            return i_get_attribute(value, pls);
        case P_SET_ATTR:
            return i_set_attribute(value, pls);
        case P_BREAK:
            return i_break();
        case P_CONTINUE:
            return i_continue();
        case P_RETURN:
            return i_return(value, pls);
        case P_NOT:
            return i_not(value, pls);
        default:
            break;
    }

    Bcode* bin = mk_safe_bcode();

    if (value->type == P_NUM) {
        bin->value.num_value = value->value.num_value;
        bin->type = OP_NUMBER;
        bin->func = bt_num;

    } else if (value->type == P_STR) {
        bin->value.str_value = value->value.str_value;
        bin->type = OP_STRING;
        bin->func = bt_str;

    } else if (value->type == P_VAR) {
        bin->value.str_value = value->value.str_value;
        bin->type = OP_VARIABLE;
        bin->line = value->line;
        bin->func = bt_get_variable;

	} else if (value->type == P_BIN_OP) {
		bin->left = i_eval_ast(value->left, pls);
		bin->right = i_eval_ast(value->right, pls);
        bin->type = OP_BINARY_OPERATION;

		if (strcmp(value->value.str_value, "+") == 0)
			bin->func = bt_add_num;
		else if (strcmp(value->value.str_value, "-") == 0)
			bin->func = bt_minus_num;
		else if (strcmp(value->value.str_value, "/") == 0)
			bin->func = bt_div_num;
		else if (strcmp(value->value.str_value, "*") == 0)
			bin->func = bt_mult_num;
       else if (strcmp(value->value.str_value, "<") == 0)
           bin->func = bt_less_than_num;
       else if (strcmp(value->value.str_value, ">") == 0)
           bin->func = bt_greater_than_num;
       else if (strcmp(value->value.str_value, "<=") == 0)
           bin->func = bt_less_equals_num;
       else if (strcmp(value->value.str_value, ">=") == 0)
           bin->func = bt_greater_equals_num;
       else if (strcmp(value->value.str_value, "==") == 0)
           bin->func = bt_equals;
       else if (strcmp(value->value.str_value, "!=") == 0)
           bin->func = bt_not_equals;
	}
    return bin;
}

Bcode* i_make_variable(Inter* pls) {
    Bcode* bin = mk_safe_bcode();
    bin->type = OP_MAKE_VARIABLE;
    bin->func = bt_make_variable;

    bin->value.str_value = pls->tok->value.str_value;
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
    bin->left = i_make_if_stat(pls);

    bin->type = OP_IF;
    bin->func = bt_if;
    
    return bin;
}

Bcode *i_make_while(Inter *pls) {
    Bcode *whl = mk_safe_bcode();
    whl->type = OP_WHILE;
    whl->func = bt_while;
    whl->left = i_eval_ast(pls->tok->left, pls);
    whl->right = i_code_to_bytecode(pls, pls->tok);

    return whl;
}

Bcode *i_make_function(Inter *pls) {
    Bcode *fn = mk_safe_bcode();
    fn->type = OP_FUNCTION;

    fn->value.str_value = pls->tok->value.str_value;
    fn->func = bt_mk_fun;

    fn->left = i_mk_list(pls->tok, pls);
    fn->right = i_code_to_bytecode(pls, pls->tok);
    return fn;
}

Bcode *i_make_class(Inter *pls) {
    Bcode *fn = mk_safe_bcode();
    fn->type = OP_CLASS;
    fn->value.str_value = pls->tok->value.str_value;
    fn->func = bt_mk_class;

    fn->left = i_mk_list(pls->tok, pls);
    fn->right = i_code_to_bytecode(pls, pls->tok);
    return fn;
}

void inter_interpret(Inter* pls) {
    while (pls->tok != NULL) {
        if (pls->tok->type == P_VAR_ASSIGN){
            append_bcode(pls, i_make_variable(pls));

        } else if (pls->tok->type == P_FN) {
            append_bcode(pls, i_make_function(pls));

        } else if (pls->tok->type == P_CLASS) {
            append_bcode(pls, i_make_class(pls));

        } else if (pls->tok->type == P_IF) {
            append_bcode(pls, i_make_if(pls));

        } else if (pls->tok->type == P_WHILE) {
            append_bcode(pls, i_make_while(pls));

        } else {
            append_bcode(pls, i_eval_ast(pls->tok, pls));

        }

        inter_advance(pls);
    }
}
