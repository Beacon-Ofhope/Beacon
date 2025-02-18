#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "Includes/lexer.h"
#include "Includes/parser.h"


Parser* parser_read(const Lexer* lex){
    Parser* pls = malloc(sizeof(Parser));
	pls->tok = lex->start;
    pls->file = lex->file;
    pls->start = NULL;
    pls->recent = NULL;

	return pls;
}

AstNode* mk_node(){
	AstNode* node = malloc(sizeof(AstNode));
	node->next = NULL;
	return node;
}

void parser_advance(Parser* pls){
    Token * temp = NULL;

	if (pls->tok != NULL) {
	    temp = pls->tok->next;
		free(pls->tok);
		pls->tok = temp;
	}
}

void parser_advance_no_free(Parser *pls){
	if (pls->tok != NULL){
		pls->tok = pls->tok->next;
	}
}

void parser_syntax_error(char* error, const Parser* pls){
  	if (pls->tok == NULL) {
  	    printf("syntaxError: %s\n\t critical: no more tokens.\n\t line: EOF, file: '%s'\n", error, pls->file);

  	} else {
    	printf("syntaxError: %s\n\t line: %d, file: '%s'\n", error, pls->tok->line, pls->file);
    }
    exit(0);
}

void append_node(Parser * pls, AstNode* new_node) {
    if (pls->start == NULL) {
        pls->start = new_node;
        pls->recent = new_node;
    } else {
        pls->recent->next = new_node;
        pls->recent = new_node;
    }
}

void append_list_node(AstNode** start, AstNode** recent, AstNode * new_node) {
    if (*start == NULL) {
        *start = new_node;
        *recent = new_node;
    } else {
        (*recent)->next = new_node;
        *recent = new_node;
    }
}

// parse shortcut for calling functions, attributes and more.
AstNode* parser_id_call(Parser* pls, char * variable){
	AstNode* node = mk_node();
	node->value.name = variable;
	node->type = P_VAR;
	node->line = (pls->tok) ? pls->tok->line: 1;

	while (pls->tok != NULL && (pls->tok->type == TK_LPAREN || pls->tok->type == TK_LBRACK || \
	pls->tok->type == TK_DOT)){
		if (pls->tok->type == TK_LPAREN){
			node = parser_call_function(node, pls);

		} else if (pls->tok->type == TK_DOT){
			node = parser_get_attribute(node, pls);
			continue;
		}

		parser_advance(pls);
	}
	return node;
}

void skip_whitespace(Parser* pls){
	if (pls->tok != NULL && pls->tok->type == TK_NL)
		parser_advance(pls);
}

AstNode* mk_list(Parser* pls){
	parser_advance(pls);

	AstNode *values = NULL;
	AstNode *recent = NULL;
	unsigned short count = 0;

	while (pls->tok != NULL && pls->tok->type != TK_RBRACK){
		switch (pls->tok->type) {
			case TK_NL:
			case TK_COMMA:
				parser_advance(pls);
				break;
			default:
				append_list_node(&values, &recent, parser_eval(pls));
				count++;
				break;
		}
	}

	if (pls->tok == NULL)
		parser_syntax_error("missing closing ']' for list", pls);

	AstNode* ast = mk_node();
	ast->type = P_LIST;
	ast->line = count;
	ast->value.data = values;

	return ast;
}

AstNode* mk_dict_face(Parser* pls, TokenType sign){
	parser_advance(pls);

	size_t capacity = 4;
	char** keys = malloc(sizeof(char*) * capacity);

	// values links
	AstNode *values = NULL;
	AstNode *recent = NULL;

	unsigned short count = 0;

	while (pls->tok != NULL && pls->tok->type != TK_RBRACE){
		switch (pls->tok->type){
			case TK_NL:
			case TK_COMMA:
				parser_advance(pls);
				break;

			default:
				if (!(pls->tok->type == TK_ID || pls->tok->type == TK_STR))
					parser_syntax_error("expects 'name' or 'string' token as dict keys.", pls);

				if (count == capacity){
					capacity *=2;
					keys = realloc(keys, sizeof(char *) * capacity);
				}

				keys[count] = pls->tok->value;
				parser_advance(pls);

				if (pls->tok->type != sign){
					if (sign == TK_COLON)
						parser_syntax_error("expected a ':' to bind key and value in dict", pls);
					else
						parser_syntax_error("expected a '=' to bind key and value in interface", pls);
				}

				parser_advance(pls);
				append_list_node(&values, &recent, parser_eval(pls));

				++count;
			break;
		}
	}

	if (pls->tok == NULL){
		free(keys);
		parser_syntax_error("missing closing '}' for dictionary", pls);
	}

	if (count == 0) {
		free(keys);
		keys = NULL;

	} else if (count != capacity){
		keys = realloc(keys, sizeof(char *) * count);
	}

	pdict* dict = malloc(sizeof(pdict));
	dict->keys = keys;
	dict->values = values;

	AstNode* ast = mk_node();
	ast->type = P_DICTIONARY;
	ast->value.dict = dict;
	ast->line = count;

	return ast;
}

AstNode* mk_interface(Parser* pls){
	parser_advance(pls);
	skip_whitespace(pls);

	if (pls->tok->type != TK_LBRACE)
		parser_syntax_error("missing starting '{' after 'interface' token", pls);

	AstNode* ast = mk_dict_face(pls, TK_EQUALS);
	ast->type = P_FACE;
	return ast;
}

AstNode *parser_get_attribute(AstNode *node, Parser *pls){
	AstNode *ast = mk_node();
	ast->line = pls->tok->line;

	parser_advance(pls);

	if (pls->tok == NULL || pls->tok->type != TK_ID)
		parser_syntax_error("expected 'name' token after '.' token", pls);

	char* name = pls->tok->value;
	parser_advance(pls);

	if (pls->tok != NULL && (pls->tok->type == TK_EQUALS)){
		psattr* set = malloc(sizeof(psattr));
		set->operator = pls->tok->value;
		set->parent = node;
		set->key = name;

		parser_advance(pls);
		set->value = parser_eval(pls);

		ast->type = P_SET_ATTR;
		ast->value.sattr = set;

	} else {
		pgattr* get = malloc(sizeof(pgattr));
		get->parent = node;
		get->key = name;

		ast->type = P_ATTR;
		ast->value.gattr = get;
	}

	return ast;
}

// collects function arguments e.g print(1,..)
AstNode* parser_get_arguments(Parser *pls, unsigned char* ref_count){
	parser_advance(pls);

	AstNode *values = NULL;
	AstNode *recent = NULL;

	while (pls->tok != NULL && pls->tok->type != TK_RPAREN){
		switch (pls->tok->type) {
			case TK_NL:
			case TK_COMMA:
				parser_advance(pls);
				break;
			default:
				append_list_node(&values, &recent, parser_eval(pls));
				++(*ref_count);
				break;
		}
	}

	if (pls->tok == NULL)
		parser_syntax_error("missing closing ')' for list", pls);

	return values;
}

AstNode *parser_call_function(AstNode *node, Parser *pls){
	AstNode *ast = mk_node();
	ast->line = pls->tok->line;

	pcall* fn = malloc(sizeof(pcall));
	fn->callee = node;

	unsigned char ref_count = 0;
	fn->args = parser_get_arguments(pls, &ref_count);
	fn->count = ref_count;

	ast->value.call = fn;
	ast->type = P_FN_CALL;

	return ast;
}

AstNode* parser_paren_precision(Parser *pls){
	parser_advance(pls);
	AstNode* node = parser_eval(pls);

	skip_whitespace(pls);

	if(pls->tok == NULL || pls->tok->type != TK_RPAREN)
		parser_syntax_error("expected closing ')' after expression in (...).", pls);

	return node;
}

AstNode* parser_not(Parser *pls){
	parser_advance(pls);

	AstNode *ast = mk_node();
	ast->type = P_NOT;
	ast->value.data = parser_factor(pls);

	return ast;
}

AstNode* parser_mk_negative(Parser *pls){
	AstNode *left = mk_node();
	left->value.number = 0;
	left->type = P_NUM;

	pbinop* bin = malloc(sizeof(pbinop));
	bin->operator = pls->tok->value;
	bin->left = left;

	AstNode* ast = mk_node();
	ast->type = P_BIN_OP;
	ast->value.binop = bin;
	ast->line = pls->tok->line;

	parser_advance(pls);
	bin->right = parser_factor(pls);

	return ast;
}

AstNode* parser_factor(Parser* pls){
	AstNode* node = mk_node();
	int skip_if_needed = 1;

	if (pls->tok == NULL || node == NULL)
    	parser_syntax_error("parsing issue, data cannot be formed.", pls);

	switch (pls->tok->type){
		case TK_ID:
			node->value.name = pls->tok->value;
			node->line = pls->tok->line;
			node->type = P_VAR;
			break;

		case TK_NUM:
			node->value.number = strtod(pls->tok->value, NULL);
			node->type = P_NUM;
			free(pls->tok->value);
			break;

		case TK_STR:
			node->value.name = pls->tok->value;
			node->type = P_STR;
			break;

		default:
			switch (pls->tok->type) {
				case TK_LBRACK:
					free(node);
					node = mk_list(pls);
					break;

				case TK_LBRACE:
					// for dictionary data
					free(node);
					node = mk_dict_face(pls, TK_COLON);
					break;
			
				case TK_LPAREN:
					free(node);
					node = parser_paren_precision(pls);
					break;
				
				case TK_ADD_SUB:
					free(node);
					node = parser_mk_negative(pls);
					skip_if_needed = 0;
					break;

				case TK_FACE:
					free(node);
					node = mk_interface(pls);
					break;

				case TK_NOT:
					free(node);
					node = parser_not(pls);
					skip_if_needed = 0;
					break;

				default:
					parser_syntax_error("Invalid syntax in expression", pls);
					break;
			}
			break;
	}

	if (skip_if_needed)
		parser_advance(pls);

	while (pls->tok != NULL && (pls->tok->type == TK_LPAREN || pls->tok->type == TK_LBRACK || \
	pls->tok->type == TK_DOT)){
		if (pls->tok->type == TK_LPAREN){
			node = parser_call_function(node, pls);
		
		} else if (pls->tok->type == TK_DOT){
			node = parser_get_attribute(node, pls);
			continue;
		}
		parser_advance(pls);
	}
	return node;
}

AstNode* parser_mult(Parser* pls){
	AstNode* left = parser_factor(pls);

	while (pls->tok != NULL && pls->tok->type == TK_MULT_DIV){
		AstNode *ast = mk_node();
		ast->line = pls->tok->line;
		ast->type = P_BIN_OP;

		pbinop *bo = malloc(sizeof(pbinop));
		bo->operator= pls->tok->value;
		parser_advance(pls);

		bo->left = left;
		bo->right = parser_add_sub(pls);

		ast->value.binop = bo;
		left = ast;
	}
	return left;
}

AstNode* parser_add_sub(Parser* pls){
	AstNode* left = parser_mult(pls);

	while (pls->tok != NULL && pls->tok->type == TK_ADD_SUB){
		AstNode *ast = mk_node();
		ast->line = pls->tok->line;
		ast->type = P_BIN_OP;

		pbinop *bo = malloc(sizeof(pbinop));
		bo->operator= pls->tok->value;
		parser_advance(pls);

		bo->left = left;
		bo->right = parser_add_sub(pls);

		ast->value.binop = bo;
		left = ast;
	}
	return left;
}

AstNode* parser_eval(Parser* pls){
	AstNode* left = parser_add_sub(pls);

	while (pls->tok != NULL && pls->tok->type == TK_GREATER_EQS){
		AstNode *ast = mk_node();
		ast->line = pls->tok->line;
		ast->type = P_BIN_OP;

		pbinop *bo = malloc(sizeof(pbinop));
		bo->operator= pls->tok->value;
		parser_advance(pls);

		bo->left = left;
		bo->right = parser_add_sub(pls);

		ast->value.binop = bo;
		left = ast;
	}

	return left;
}

AstNode* parse_variable(Parser* pls){
	char *name = (pls->tok->value);
	unsigned short line = pls->tok->line;
	parser_advance(pls);

	if (pls->tok == NULL || pls->tok->type == TK_NL){
		AstNode *ast = mk_node();
		ast->type = P_VAR;
		ast->value.name = name;
		ast->line = line;

		return ast;
	}

	if (pls->tok != NULL && pls->tok->type == TK_EQUALS){
		parser_advance(pls);

		pmkvar* var = malloc(sizeof(pmkvar));
		var->value = parser_eval(pls);
		var->key = name;

		AstNode *ast = mk_node();
		ast->type = P_VAR_ASSIGN;
		ast->line = line;
		ast->value.mkvar = var;

		return ast;

	} else if (pls->tok != NULL && (pls->tok->type == TK_LPAREN || pls->tok->type == TK_DOT)){
		return parser_id_call(pls, name);

	}

	parser_syntax_error("unexpected syntax when assigning data to a variable", pls);
	return NULL;
}

void parser_append_block_token(Token **start, Token **recent, Token *new_node){
	if (*start == NULL)	{
		*start = new_node;
		*recent = new_node;
	} else {
		(*recent)->next = new_node;
		*recent = new_node;
	}
}

Token* copy_token(Parser* pls){
	Token *tok = pls->tok;
	parser_advance_no_free(pls);

	tok->next = NULL;
	return tok;
}

void collect_single_block(Parser *pls, Token **start, Token **recent){
	while (pls->tok != NULL && pls->tok->type != TK_NL) {
		parser_append_block_token(start, recent, copy_token(pls));
	}
}

void collect_mult_block(Parser *pls, Token **start, Token **recent){
	skip_whitespace(pls);
	
	if (pls->tok == NULL && pls->tok->type != TK_LBRACE)
		parser_syntax_error("expected '{' token to get code block.", pls);

	parser_advance(pls);
	int count = 1;

	while (pls->tok != NULL && count > 0){
		if (pls->tok->type == TK_LBRACE)
			count++;
		else if (pls->tok->type == TK_RBRACE)
			count--;

		if (count > 0){
			parser_append_block_token(start, recent, copy_token(pls));
		}
	}

	if (pls->tok == NULL)
		parser_syntax_error("expected closing '}' but reached EOF", pls);

	parser_advance(pls);
}

AstNode* parser_collect_block(Parser* pls){
	// TOKENS
	Token *start = NULL;
	Token *recent = NULL;

	// single line block
	if (pls->tok != NULL && (pls->tok->type != TK_NL && pls->tok->type != TK_LBRACE))
		collect_single_block(pls, &start, &recent);

	else {
		if (pls->tok != NULL && pls->tok->type == TK_NL){
			parser_advance(pls);

			if (pls->tok != NULL && (pls->tok->type != TK_NL && pls->tok->type != TK_LBRACE))
				collect_single_block(pls, &start, &recent);
			else
				collect_mult_block(pls, &start, &recent);

		} else {
			collect_mult_block(pls, &start, &recent);
		}
	}

	Lexer *lex = calloc(1, sizeof(struct LEX_UP));
	lex->start = start;
	lex->file = pls->file;

	Parser *par = parser_read(lex);
	parser_tree(par);
	free(lex);

	AstNode* nodes = par->start;
	free(par);

	return nodes;
}

void add_strings_array(ptrack* trace, char* st){
	char** stats = (char**)(trace->pointer);

	if (trace->capacity == trace->count){
		trace->capacity += 5;
		stats = realloc(stats, sizeof(char*) * trace->capacity);
	}

	stats[trace->count] = st;
	++(trace->count);
	trace->pointer = (void*)stats;
}

ptrack* parser_get_parameters(Parser *pls){
	if (!(pls->tok) || pls->tok->type != TK_LPAREN)
		parser_syntax_error("Expects '(' token to get function parameters.", pls);

	parser_advance(pls);
	char **params = malloc(sizeof(char *) * 4);

	ptrack *trace = malloc(sizeof(ptrack));
	trace->pointer = (void *)params;
	trace->capacity = 4;
	trace->count = 0;

	while (pls->tok && pls->tok->type != TK_RPAREN){
		switch (pls->tok->type) {
			case TK_NL:
			case TK_COMMA:
				parser_advance(pls);
				break;
			case TK_ID:
				add_strings_array(trace, pls->tok->value);
				parser_advance(pls);
				break;
			default:
				parser_syntax_error("Invalid syntax in function parameters", pls);
				break;
		}
	}

	if (!(pls->tok))
		parser_syntax_error("missing closing ')' token in function parameters.", pls);
	
	parser_advance(pls);

	if (trace->count < trace->capacity)
		params = realloc(params, sizeof(char *) * trace->count);

	return trace;
}

AstNode* parse_function(Parser* pls){
	parser_advance(pls);

	if (pls->tok == NULL || pls->tok->type != TK_ID)
		parser_syntax_error("expected an 'name' token after 'fun' token", pls);

	pmkfun* fun = malloc(sizeof(pmkfun));
	fun->name = pls->tok->value;
	parser_advance(pls);

	ptrack* trace = parser_get_parameters(pls);
	unsigned char args_count = trace->count;

	if (trace->count > 0)
		fun->params = (char **)(trace->pointer);

	free(trace);
	fun->code_block = parser_collect_block(pls);

	AstNode* ast = mk_node();
	ast->type = P_FN;
	ast->value.mkfun = fun;
	ast->line = args_count;

	return ast;
}

AstNode *parse_return(Parser *pls){
	AstNode *bin = mk_node();
	bin->line = pls->tok->line;
	bin->type = P_RETURN;

	parser_advance(pls);

	if (pls->tok && pls->tok->type != TK_NL)
		bin->value.data = parser_eval(pls);

	else {
		AstNode *none = mk_node();
		none->type = P_NONE;
		bin->value.data = none;
	}

	return bin;
}

AstNode* parser_get_extends(Parser * pls){
	if (pls->tok == NULL)
		parser_syntax_error("Unexpected syntax in class after 'name' token", pls);

	else if (pls->tok->type != TK_LPAREN)
		return NULL;

	parser_advance(pls);

	AstNode *inherits = NULL;
	AstNode *recent = NULL;	

	while (pls->tok != NULL && pls->tok->type != TK_RPAREN){
		switch (pls->tok->type){
			case TK_NL:
			case TK_COMMA:
				parser_advance(pls);
				break;
			default:
				append_list_node(&inherits, &recent, parser_eval(pls));
				break;
		}
	}

    if (pls->tok == NULL) 
        parser_syntax_error("missing closing ')' token in class", pls);
    
	parser_advance(pls);
	return inherits;
}

AstNode* parse_class(Parser* pls){
	parser_advance(pls);

	if (pls->tok->type != TK_ID)
		parser_syntax_error("expected a 'name' token after 'class' token", pls);

	pmkclass* pcls = malloc(sizeof(pmkclass));
	pcls->name = pls->tok->value;
	parser_advance(pls);

	pcls->exts = parser_get_extends(pls);
	pcls->code_block = parser_collect_block(pls);

	AstNode *ast = mk_node();
	ast->value.mkclass = pcls;
	ast->type = P_CLASS;

	return ast;
}

pwhile *parser_if_else(Parser* pls){
	parser_advance(pls);

	pwhile* stat = malloc(sizeof(pwhile));
	stat->condition = parser_eval(pls);
	stat->code_block = parser_collect_block(pls);

	return stat;
}

pwhile* parser_else(Parser *pls){
	parser_advance(pls);
	
	AstNode *term = mk_node();
	term->type = P_NUM;
	term->value.number = 1;

	pwhile *stat = malloc(sizeof(pwhile));
	stat->condition = term;
	stat->code_block = parser_collect_block(pls);

	return stat;
}

void add_pwhile_stat(ptrack* trace, pwhile* st){
	pwhile** stats = (pwhile**)(trace->pointer);

	if (trace->capacity == trace->count){
		trace->capacity += 5;
		stats = realloc(stats, sizeof(pwhile *) * trace->capacity);
	}

	stats[trace->count] = st;
	++(trace->count);
	trace->pointer = (void*)stats;
}

AstNode* parse_if(Parser* pls){
	// statements
	pwhile** stats = malloc(sizeof(pwhile*) * 5);

	ptrack *trace = malloc(sizeof(ptrack));
	trace->pointer = (void*)stats;
	trace->capacity = 5;
	trace->count = 0;

	add_pwhile_stat(trace, parser_if_else(pls));
	skip_whitespace(pls);
	int stop = 1;

	while (pls->tok != NULL && stop && (pls->tok->type == TK_ELIF || pls->tok->type == TK_ELSE || pls->tok->type == TK_NL)){
		switch (pls->tok->type){
			case TK_ELSE:
				add_pwhile_stat(trace, parser_else(pls));
				stop = 0;
				break;
			case TK_NL:
				parser_advance(pls);
				break;
			default:
				add_pwhile_stat(trace, parser_if_else(pls));
				break;
		}
	}

	if (trace->count < trace->capacity)
		stats = realloc(stats, sizeof(pwhile *) * trace->count);

	AstNode* bin = mk_node();
	bin->type = P_IF;
	bin->value.pif = stats; // stores all if states
	bin->line = trace->count; // no. of if stats is line

	return bin;
}

AstNode *parse_while(Parser *pls) {
	parser_advance(pls);

	pwhile* wl = malloc(sizeof(pwhile));
	wl->condition = parser_eval(pls);
	wl->code_block = parser_collect_block(pls);

	AstNode *ast = mk_node(pls);
	ast->value.pwhile = wl;
	ast->type = P_WHILE;

	return ast;
}

AstNode *parse_break(Parser *pls){
	AstNode *ast = mk_node();
	ast->type = P_BREAK;
	ast->line = pls->tok->line;
	parser_advance(pls);
	return ast;
}

AstNode *parse_continue(Parser *pls){
	AstNode *ast = mk_node();
	ast->type = P_CONTINUE;
	ast->line = pls->tok->line;
	parser_advance(pls);
	return ast;
}

AstNode* parse_try(Parser* pls){
	parser_advance(pls);

	ptry* ty = malloc(sizeof(ptry));
	ty->tried = parser_collect_block(pls);

	skip_whitespace(pls);

	if (!(pls->tok) || pls->tok->type != TK_CATCH){
		ty->retry = NULL;

	} else {
		parser_advance(pls);

		if (pls->tok && pls->tok->type == TK_ID){
			ty->error_name = pls->tok->value;
			parser_advance(pls);

		} else
			ty->error_name = NULL;

		ty->retry = parser_collect_block(pls);
	}

	AstNode *ast = mk_node();
	ast->value.ptry = ty;
	ast->type = P_TRY;

	return ast;
}

AstNode *parse_throw(Parser *pls){
	AstNode *ast = mk_node();
	ast->line = pls->tok->line;
	parser_advance(pls);

	ast->value.data = parser_eval(pls);
	ast->type = P_THROW;

	return ast;
}

AstNode *parse_import(Parser *pls) {
	unsigned short line = pls->tok->line;
	parser_advance(pls);

	if (pls->tok == NULL)
		parser_syntax_error("no more tokens for import statement", pls);

	unsigned char count = 0;
	unsigned char capacity = 3;
	pimport *imp = malloc(sizeof(pimport));

	if (pls->tok->type == TK_LBRACE){
		parser_advance(pls);
		pport **dirs = malloc(sizeof(pport *) * capacity);

		while (pls->tok && pls->tok->type != TK_RBRACE){
			switch (pls->tok->type) {
				case TK_NL:
				case TK_COMMA:
					parser_advance(pls);
					break;

				case TK_ID:
				case TK_STR:
					pport *di = malloc(sizeof(pport));
					di->name = pls->tok->value;
					parser_advance(pls);

					if (pls->tok == NULL || pls->tok->type != TK_AS)
						di->alias = di->name;
					else {
						parser_advance(pls);

						if (pls->tok == NULL || pls->tok->type != TK_ID)
							parser_syntax_error("expects 'name' token after 'as' in import statement", pls);
						
						di->alias = pls->tok->value;
						parser_advance(pls);
					}
					
					if (count == capacity){
						capacity *= 2;
						dirs = realloc(dirs, sizeof(pport *) * capacity);
					}

					dirs[count] = di;
					++count;
					break;			
				default:
					parser_syntax_error("unexpected syntax in import statement.", pls);
					break;
			}
		}

		if (pls->tok == NULL){
			free(dirs);
			free(imp);
			parser_syntax_error("missing closing '}' in import statement.", pls);
		}
		parser_advance(pls);

		if (count == 0) {
			free(imp);
			parser_syntax_error("expected some 'dirs' to import from 'path'.", pls);
			
		} else if (count < capacity) {
			dirs = realloc(dirs, sizeof(pport *) * count);
		}
		
		imp->count = count;
		imp->imports.dirs = dirs;
	
	} else {
		if (!(pls->tok && (pls->tok->type == TK_ID || pls->tok->type == TK_STR || pls->tok->type == TK_MULT_DIV)))
			parser_syntax_error("expected 'name' token after 'import' in import statement.", pls);
		else {
			pport *di = malloc(sizeof(pport));

			switch (pls->tok->type) {
				case TK_MULT_DIV:
					di->name = "*";
					break;
				default:
					di->name = pls->tok->value;
					break;
			}

			parser_advance(pls);

			if (!(pls->tok && pls->tok->type == TK_AS))
				di->alias = di->name;
			else {
				parser_advance(pls);

				if (!(pls->tok) || pls->tok->type != TK_ID)
					parser_syntax_error("expects 'name' token after 'as' in import statement", pls);
				
				di->alias = pls->tok->value;
				parser_advance(pls);
			}
			imp->imports.dir = di;
			imp->count = 0;
		}
	}

	if (pls->tok == NULL || pls->tok->type == TK_NL)
		imp->path = NULL;
	else {
		if (pls->tok->type != TK_FROM)
			parser_syntax_error("expects 'from' token after dirs in import statement", pls);

		parser_advance(pls);

		if (pls->tok == NULL || (pls->tok->type != TK_STR && pls->tok->type != TK_ID))
			parser_syntax_error("expects a ('string' or 'name') token after 'from' in import statement", pls);

		imp->path = pls->tok->value;
		parser_advance(pls);
	}

	AstNode* ast = mk_node();
	ast->line = line;
	ast->type = P_IMPORT;
	ast->value.pimport = imp;

	return ast;
}

void parser_tree(Parser* pls) {
    while (pls->tok) {
		switch (pls->tok->type) {
			case TK_NL:
				parser_advance(pls);
				break;
				
			case TK_ID:
				append_node(pls, parse_variable(pls));
				break;

			case TK_IF:
				append_node(pls, parse_if(pls));
				break;

			default:
				switch (pls->tok->type){
					case TK_RETURN:
						append_node(pls, parse_return(pls));
						break;

					case TK_FUN:
						append_node(pls, parse_function(pls));
						break;

					case TK_WHILE:
						append_node(pls, parse_while(pls));
						break;

					case TK_CLASS:
						append_node(pls, parse_class(pls));
						break;

					case TK_IMPORT:
						append_node(pls, parse_import(pls));
						break;

					case TK_BREAK:
						append_node(pls, parse_break(pls));
						break;

					case TK_CONTINUE:
						append_node(pls, parse_continue(pls));
						break;

					case TK_THROW:
						append_node(pls, parse_throw(pls));
						break;

					case TK_TRY:
						append_node(pls, parse_try(pls));
						break;

					case TK_PASS:
						parser_advance(pls);
						break;

					default:
						append_node(pls, parser_eval(pls));
						parser_advance(pls);
						break;
				}
				break;
		}
	}
}
