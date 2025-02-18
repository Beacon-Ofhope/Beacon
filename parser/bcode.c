#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Includes/bcode.h"
#include "Includes/bobject.h"
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

Bcode* mk_bcode() {
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
    pcall *f = tok->value.call;

    opcall* call = malloc(sizeof(opcall));
    call->callee = i_eval_ast(f->callee, pls);
    call->args = (f->count) ? i_code_block(pls, f->args) : NULL;
    call->count = f->count;

    bargs *argv = malloc(sizeof(bargs));
    argv->count = f->count;
    argv->line = tok->line;

    if (f->count)
        argv->argv = (f->count) ? (calloc((size_t)(f->count), sizeof(Bobject *))) : NULL;

    call->argv = argv;
    
    Bcode *op = mk_bcode();
    op->type = OP_FUNCTION_CALL;
    op->func = bt_call_function;
    op->line = tok->line;
    op->value.call = call;

    return op;
}

Bcode* i_mk_list(AstNode* tok, Inter* pls){
    Bcode *op = mk_bcode();
    op->value.data = i_code_block(pls, tok->value.data);
    op->line = tok->line;
    op->type = OP_LIST;
    op->func = bt_list;

    return op;
}

Bcode* i_mk_dict(AstNode* tok, Inter* pls){
    pdict *d = tok->value.dict;

    opdict *dt = malloc(sizeof(opdict));
    dt->keys = d->keys;
    dt->values = i_code_block(pls, d->values);

    Bcode *op = mk_bcode();
    op->type = OP_DICTIONARY;
    op->func = bt_dict;
    op->value.dict = dt;
    op->line = tok->line;

    return op;
}

Bcode* i_mk_interface(AstNode* tok, Inter* pls){
    Bcode *op = i_mk_dict(tok, pls);
    op->type = OP_FACE;
    op->func = bt_face;

    return op;
}

Bcode* i_get_attribute(AstNode* tok, Inter * pls){
    pgattr *g = tok->value.gattr;

    opgattr *get = malloc(sizeof(opgattr));
    get->key = g->key;
    get->parent = i_eval_ast(g->parent, pls);

    Bcode* op = mk_bcode();
    op->type = OP_GET_ATTRIBUTE;
    op->func = bt_get_attribute;
    op->value.pget = get;
    op->line = tok->line;

    return op;
}

Bcode* i_set_attribute(AstNode* tok, Inter * pls){
    psattr *s = tok->value.sattr;

    opsattr *set = malloc(sizeof(opsattr));
    set->key = s->key;
    set->operator = s->operator;
    set->parent = i_eval_ast(s->parent, pls);
    set->value = i_eval_ast(s->value, pls);

    Bcode* op = mk_bcode();
    op->type = OP_SET_ATTRIBUTE;
    op->func = bt_set_attribute;
    op->value.pset = set;
    op->line = tok->line;

    return op;
}

Bcode* i_break(){
    Bcode *bin = mk_bcode();
    bin->type = OP_BREAK;
    bin->func = bt_break;

    return bin;
}

Bcode* i_continue(){
    Bcode *bin = mk_bcode();
    bin->type = OP_CONTINUE;
    bin->func = bt_continue;

    return bin;
}

Bcode* i_return(AstNode *tok, Inter *pls){
    Bcode *op = mk_bcode();
    op->type = OP_RETURN;
    op->value.data =i_eval_ast(tok->value.data, pls);
    op->func = bt_return;

    return op;
}

Bcode* i_throw(AstNode *tok, Inter *pls){
    Bcode *op = mk_safe_bcode();
    op->type = OP_THROW;
    op->value.data = i_eval_ast(tok->value.data, pls);
    op->func = bt_throw;

    return op;
}

Bcode* i_not(AstNode *tok, Inter *pls){
    Bcode *op = mk_bcode();
    op->value.data = i_eval_ast(tok->value.data, pls);
    op->func = bt_not;
    op->type = OP_NOT;

    return op;
}

Bcode* i_eval_ast(AstNode* value, Inter * pls){
    switch (value->type){
        case P_FN_CALL:
            return i_call_function(value, pls);
        case P_ATTR:
            return i_get_attribute(value, pls);
        case P_SET_ATTR:
            return i_set_attribute(value, pls);
        case P_LIST:
            return i_mk_list(value, pls);
        case P_DICTIONARY:
            return i_mk_dict(value, pls);
        case P_FACE:
            return i_mk_interface(value, pls);
        case P_BREAK:
            return i_break();
        case P_CONTINUE:
            return i_continue();
        case P_RETURN:
            return i_return(value, pls);
        case P_THROW:
            return i_throw(value, pls);
        case P_NOT:
            return i_not(value, pls);
    }

    Bcode* bin = mk_bcode();

    switch (value->type) {
        case P_VAR:
            bin->value.name = value->value.name;
            bin->type = OP_VARIABLE;
            bin->line = value->line;
            bin->func = bt_get_variable;
            break;

        case P_NUM:
            bin->value.number = value->value.number;
            bin->type = OP_NUMBER;
            bin->func = bt_num;
            break;

        case P_STR:
            bin->value.name = value->value.name;
            bin->type = OP_STRING;
            bin->func = bt_str;
            break;

        default:
            switch (value->type){
                case P_BIN_OP:
                    pbinop *b = value->value.binop;

                    opbinop *bp = malloc(sizeof(opbinop));
                    bp->left = i_eval_ast(b->left, pls);
                    bp->right = i_eval_ast(b->right, pls);

                    bin->type = OP_BINARY_OPERATION;
                    bin->value.binop = bp;

                    if (strcmp(b->operator, "+") == 0)
                        bin->func = bt_add_num;

                    else if (strcmp(b->operator, "-") == 0)
                        bin->func = bt_minus_num;

                    else if (strcmp(b->operator, "/") == 0)
                        bin->func = bt_div_num;

                    else if (strcmp(b->operator, "*") == 0)
                        bin->func = bt_mult_num;

                    else if (strcmp(b->operator, "%") == 0)
                        bin->func = bt_mod_num;

                    else if (strcmp(b->operator, "<") == 0)
                        bin->func = bt_less_than_num;

                    else if (strcmp(b->operator, ">") == 0)
                        bin->func = bt_greater_than_num;

                    else if (strcmp(b->operator, "<=") == 0)
                        bin->func = bt_less_equals_num;

                    else if (strcmp(b->operator, ">=") == 0)
                        bin->func = bt_greater_equals_num;

                    else if (strcmp(b->operator, "==") == 0)
                        bin->func = bt_equals;

                    else if (strcmp(b->operator, "!=") == 0)
                        bin->func = bt_not_equals;
                    break;

                case P_NONE:
                    bin->type = OP_NONE;
                    bin->func = bt_none;
                    break;

                default:
                    break;
            }
    }

    return bin;
}

Bcode* i_make_variable(Inter* pls) {
    Bcode *op = mk_bcode();
    op->type = OP_MAKE_VARIABLE;
    op->func = bt_make_variable;
    op->line = pls->tok->line;

    pmkvar *v = pls->tok->value.mkvar;
    
    opmkvar *var = malloc(sizeof(opmkvar));
    var->key = v->key;
    var->value = i_eval_ast(v->value, pls);

    op->value.mkvar = var;
    return op;
}

Bcode *i_make_if(Inter *pls) {
    unsigned short count = pls->tok->line;

    opwhile **ifs = calloc(count, sizeof(opwhile *));
    pwhile **if_ast = pls->tok->value.pif;

    opwhile *stat;

    for(unsigned short i = 0; i < count; ++i){
        stat = malloc(sizeof(opwhile));
        stat->condition = i_eval_ast(if_ast[i]->condition, pls);
        stat->code_block = i_code_block(pls, if_ast[i]->code_block);
        ifs[i] = stat;
    }

    Bcode *op = mk_bcode();
    op->type = OP_IF;
    op->value.pif = ifs;
    op->line = count;
    op->func = bt_if;

    return op;
}

Bcode *i_code_block(Inter *pls, AstNode *ast){
    Parser *par = calloc(1, sizeof(Parser));
    par->file = pls->file;
    par->start = ast;

    Inter *b_interpreter = inter_read(par);
    inter_interpret(b_interpreter);

    return b_interpreter->start;
}

Bcode *i_make_while(Inter *pls) {
    opwhile* wl = malloc(sizeof(opwhile));
    wl->condition = i_eval_ast(pls->tok->value.pwhile->condition, pls);
    wl->code_block = i_code_block(pls, pls->tok->value.pwhile->code_block);

    Bcode *op = mk_bcode();
    op->type = OP_WHILE;
    op->func = bt_while;
    op->value.pwhile = wl;

    return op;
}

Bcode *i_make_function(Inter *pls) {
    pmkfun *f = pls->tok->value.mkfun;

    opmkfun *fn = malloc(sizeof(opmkfun));
    fn->name = f->name;
    fn->params = f->params;
    fn->code_block = i_code_block(pls, f->code_block);

    Bcode *op = mk_bcode();
    op->line = pls->tok->line;
    op->type = OP_FUNCTION;
    op->func = bt_mk_fun;
    op->value.pmkfun = fn;

    return op;
}

Bcode *i_make_class(Inter *pls) {
    pmkclass *c = pls->tok->value.mkclass;

    opmkclass *cl = malloc(sizeof(opmkclass));
    cl->name = c->name;
    cl->exts = i_code_block(pls, c->exts);
    cl->code_block = i_code_block(pls, c->code_block);

    Bcode *op = mk_bcode();
    op->type = OP_CLASS;
    op->func = bt_mk_class;
    op->value.pmkclass = cl;

    return op;
}

Bcode *i_make_try(Inter *pls) {
    ptry *t = pls->tok->value.ptry;

    optry *ty = malloc(sizeof(optry));
    ty->error_name = t->error_name;
    ty->tried = i_code_block(pls, t->tried);
    ty->retry = (t->retry == NULL) ? NULL : i_code_block(pls, t->retry);

    Bcode *op = mk_bcode();
    op->func = bt_try;
    op->type = OP_TRY_CATCH;
    op->value.ptry = ty;

    return op;
}

Bcode *i_make_import(Inter *pls){
    Bcode *op = mk_bcode();
    op->value.pimport = pls->tok->value.pimport;
    op->line = pls->tok->line;
    op->type = OP_IMPORT;
    op->func = bt_import;

    return op;
}

void inter_interpret(Inter* pls) {
    while (pls->tok != NULL) {
        switch (pls->tok->type) {
            case P_VAR_ASSIGN:
                append_bcode(pls, i_make_variable(pls));
                break;

            case P_IF:
                append_bcode(pls, i_make_if(pls));
                break;

            case P_FN:
                append_bcode(pls, i_make_function(pls));
                break;

            case P_CLASS:
                append_bcode(pls, i_make_class(pls));
                break;

            case P_IMPORT:
                append_bcode(pls, i_make_import(pls));
                break;

            case P_WHILE:
                append_bcode(pls, i_make_while(pls));
                break;

            case P_TRY:
                append_bcode(pls, i_make_try(pls));
                break;

            default:
                append_bcode(pls, i_eval_ast(pls->tok, pls));
                break;
        }

        inter_advance(pls);
    }
}

