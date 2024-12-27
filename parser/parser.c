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

AstNode* mk_safe_node(){
	AstNode *node = calloc(sizeof(AstNode), 1);
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
  	    printf("Syntax_error: %s\n\t critical: no more tokens.\n\t line: EOF, file: '%s'\n", error, pls->file);

  	} else {
    	printf("Syntax_error: %s\n\t line: %d, file: '%s'\n", error, pls->tok->line, pls->file);
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

// collects function arguments e.g print(1,..)
AstNode* parser_get_arguments(Parser *pls){
	parser_advance(pls);
	AstNode *start_arg = NULL;
	AstNode *recent_arg = NULL;
	AstNode *def_param = NULL;

	bool alread_has_defaults = false;

	while (pls->tok != NULL && pls->tok->type != TK_RPAREN){
		if (pls->tok->type == TK_NL || pls->tok->type == TK_COMMA){
			parser_advance(pls);

		} else {
            if (pls->tok->type == TK_ID){
                def_param = parser_eval(pls);

                if (pls->tok != NULL && pls->tok->type == TK_EQUALS){
                    if(def_param->type != P_VAR){
						free(def_param);
						parser_syntax_error("Invalid syntax in function arguments", pls);
                    }

                    parser_advance(pls);
                    AstNode* mk_def_param_var = mk_safe_node();
                    mk_def_param_var->type = P_VAR_ASSIGN;
					mk_def_param_var->value.str_value = def_param->value.str_value;
                    mk_def_param_var->left = parser_eval(pls);

					free(def_param);
					append_list_node(&start_arg, &recent_arg, mk_def_param_var);
					alread_has_defaults = true;

                } else {
					if (alread_has_defaults){
						parser_syntax_error("cannot follow assigned parameter by a non assigned one", pls);
					}

                    append_list_node(&start_arg, &recent_arg, def_param);
                }
			} else {
				if (alread_has_defaults){
					parser_syntax_error("cannot follow assigned parameter by a non assigned one", pls);
				}
			
				append_list_node(&start_arg, &recent_arg, parser_eval(pls));
			}
		}
	}

    if (pls->tok == NULL) {
        parser_syntax_error("invalid syntax, missing closing ')' token in fun call.", pls);
    }
	return start_arg;
}

// parse shortcut for calling functions, attributes and more.
AstNode* parser_id_call(Parser* pls, char * variable){
	AstNode* node = mk_safe_node();
	node->value.str_value = (variable);
	node->type = P_VAR;

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

void clean_token_string(const Parser * pls, AstNode * node) {
    node->value.str_value = (char*)malloc(strlen(pls->tok->value) + 1);
    strcpy(node->value.str_value, pls->tok->value);

    free(pls->tok->value);
}

void skip_whitespace(Parser* pls){
	if (pls->tok != NULL && pls->tok->type == TK_NL)
		parser_advance(pls);
}

AstNode* mk_list(Parser* pls){
	parser_advance(pls);

	AstNode *list_data = NULL;
	AstNode *recent = NULL;

	while (pls->tok != NULL && pls->tok->type != TK_RBRACK){
		if (pls->tok->type == TK_NL || pls->tok->type == TK_COMMA) {
			parser_advance(pls);

		} else {
				append_list_node(&list_data, &recent, parser_eval(pls));
		}
	}

	if (pls->tok == NULL){
		parser_syntax_error("Invalid syntax, no closing ']' for list", pls);
	}

	AstNode* list = mk_safe_node();
	list->type = P_LIST;
	list->left = list_data;

	return list;
}

AstNode *parser_get_attribute(AstNode *node, Parser *pls){
	parser_advance(pls);

	if (pls->tok == NULL || pls->tok->type != TK_ID){
		parser_syntax_error("Invalid syntax, Expected name token for object attribute", pls);
	}

	AstNode *bin = mk_safe_node();
	bin->left = node;
	bin->value.str_value = pls->tok->value;

	parser_advance(pls);

	if (pls->tok != NULL && (pls->tok->type == TK_EQUALS)){
		parser_advance(pls);

		bin->type = P_SET_ATTR;
		bin->right = parser_eval(pls);

	} else {
		bin->type = P_ATTR;
	}

	return bin;
}

AstNode *parser_call_function(AstNode *node, Parser *pls){
	AstNode *data = mk_safe_node();
	data->type = P_FN_CALL;
	data->left = node;
	data->line = pls->tok->line;
	data->right = parser_get_arguments(pls);

	return data;
}

AstNode* parser_paren_precision(Parser *pls){
	parser_advance(pls);
	AstNode* node = parser_eval(pls);

	skip_whitespace(pls);

	if(pls->tok == NULL || pls->tok->type != TK_RPAREN)
		parser_syntax_error("Invalid syntax, expected closing ')' at paren precision.", pls);

	return node;
}

AstNode* parser_not(Parser *pls){
	parser_advance(pls);
	AstNode *bin = mk_safe_node();
	bin->type = P_NOT;
	bin->left = parser_eval(pls);

	return bin;
}

AstNode* parser_factor(Parser* pls){
	AstNode* node = mk_safe_node();
	int skip_if_needed = 1;

	if (pls->tok == NULL || node == NULL)
    	parser_syntax_error("parsing issue, data cannot be formed.", pls);

	if (pls->tok->type == TK_NUM) {
        node->value.num_value = strtod(pls->tok->value, NULL);
	    free(pls->tok->value);
		node->type = P_NUM;

	} else if (pls->tok->type == TK_STR){
		node->value.str_value = pls->tok->value;
		node->type = P_STR;

	} else if (pls->tok->type == TK_ID){
		node->value.str_value = pls->tok->value;
		node->line = pls->tok->line;
		node->type = P_VAR;

	} else if (pls->tok->type == TK_TRUE){
		node->type = P_TRUE;
	
	} else if (pls->tok->type == TK_FALSE){
		node->type = P_FALSE;
	
	} else if (pls->tok->type == TK_NONE){
		node->type = P_NONE;
	
	} else if (pls->tok->type == TK_LBRACK){
		free(node);
		node = mk_list(pls);
	
	} else if (pls->tok->type == TK_LPAREN){
		free(node);
		node = parser_paren_precision(pls);

	} else if (pls->tok->type == TK_NOT) {
		free(node);
		node = parser_not(pls);
		skip_if_needed = 0;

	} else {
		parser_syntax_error("Invalid syntax in expression", pls);

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
		AstNode *bin = mk_safe_node();
		bin->value.str_value = pls->tok->value;
		bin->type = P_BIN_OP;
		bin->left = left;

		parser_advance(pls);
		bin->right = parser_factor(pls);
		left = bin;
	}
	return left;
}

AstNode* parser_add_sub(Parser* pls){
	AstNode* left = parser_mult(pls);

	while (pls->tok != NULL && pls->tok->type == TK_ADD_SUB){
		AstNode *bin = mk_safe_node();
		bin->value.str_value = pls->tok->value;
		bin->type = P_BIN_OP;
		bin->left = left;

		parser_advance(pls);

		bin->right = parser_mult(pls);
		left = bin;
	}
	return left;
}

AstNode* parser_eval(Parser* pls){
	AstNode* left = parser_add_sub(pls);

	while (pls->tok != NULL && pls->tok->type == TK_GREATER_EQS){
		AstNode *bin = mk_safe_node();
		bin->value.str_value = pls->tok->value;
		bin->type = P_BIN_OP;
		bin->left = left;

		parser_advance(pls);
		bin->right = parser_add_sub(pls);
		left = bin;
	}
	return left;
}

AstNode* parse_variable(Parser* pls){
	char *name = (pls->tok->value);
	parser_advance(pls);

	if (pls->tok == NULL || pls->tok->type == TK_NL){
		AstNode *ast = mk_safe_node();
		ast->type = P_VAR;
		ast->value.str_value = name;

		return ast;
	}

	if (pls->tok != NULL && pls->tok->type == TK_EQUALS){
		parser_advance(pls);

		if (pls->tok == NULL){
			parser_syntax_error("unexpected syntax when assigning data to a variable", pls);
		}

		AstNode *ast = mk_safe_node();

		ast->type = P_VAR_ASSIGN;
		ast->value.str_value = name;
		ast->left = parser_eval(pls);

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
		parser_syntax_error("Expected a closing '}' but reached EOF", pls);

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

AstNode* parser_get_parameters(Parser * pls){
	parser_advance(pls);
	AstNode* args = NULL;
	AstNode* recent = NULL;

    if (pls->tok == NULL)
    	parser_syntax_error("Invalid syntax, cannot collect arguments", pls);

	while (pls->tok != NULL && pls->tok->type != TK_RPAREN){
		if (pls->tok->type == TK_NL || pls->tok->type == TK_COMMA){
			parser_advance(pls);

		} else {
            if (pls->tok->type == TK_ID){
                AstNode *def_param = parser_eval(pls);

                if (pls->tok != NULL && pls->tok->type == TK_EQUALS){
                    if(def_param->type != P_VAR){
						free(def_param);
                        parser_syntax_error("Invalid syntax in function arguments", pls);
                    }

                    parser_advance(pls);
                    AstNode* mk_def_var = mk_safe_node();
                    mk_def_var->type = P_VAR_ASSIGN;
					mk_def_var->value.str_value = (def_param->value.str_value);
                    mk_def_var->left = parser_eval(pls);

					free(def_param);
					append_list_node(&args, &recent, mk_def_var);

                } else {
                    append_list_node(&args, &recent, def_param);
                }
			} else {
				parser_syntax_error("Invalid syntax in function declartion parameters", pls);
			}
		}
	}

    if (pls->tok == NULL) 
        parser_syntax_error("invalid syntax, missing closing ')' token after collecting parameters.", pls);
    
	parser_advance(pls);
	return args;
}

AstNode* parse_function(Parser* pls){
	parser_advance(pls);

	if (pls->tok->type != TK_ID)
		parser_syntax_error("unexpected syntax after 'fun' token, expected an ID token", pls);

	AstNode* ast = mk_safe_node();
	ast->value.str_value = pls->tok->value;
	parser_advance(pls);
	ast->left = parser_get_parameters(pls);
	ast->right = parser_collect_block(pls);
	ast->type = P_FN;

	return ast;
}

AstNode *parse_return(Parser *pls){
	parser_advance(pls);

	AstNode *bin = mk_safe_node();
	bin->type = P_RETURN;
	bin->left = parser_eval(pls);
	
	return bin;
}

AstNode* parse_class(Parser* pls){
	parser_advance(pls);

	if (pls->tok->type != TK_ID)
		parser_syntax_error("unexpected syntax after 'class' token, expected an ID token", pls);

	AstNode* ast = mk_safe_node();
	ast->value.str_value = pls->tok->value;
	parser_advance(pls);

	if (pls->tok != NULL && pls->tok->type == TK_LPAREN)
		ast->left = parser_get_parameters(pls);
	
	ast->right = parser_collect_block(pls);
	ast->type = P_CLASS;

	return ast;
}

AstNode *parser_if_else(Parser* pls){
	parser_advance(pls);

	AstNode *bin = mk_safe_node();

	bin->left = parser_eval(pls);
	bin->right = parser_collect_block(pls);
	bin->type = P_IF_ELSE;

	return bin;
}

AstNode *mk_true(){
	AstNode* node = mk_safe_node();
	node->type = P_NUM;
	node->value.num_value = 1;

	return node;
}

AstNode* parser_else(Parser *pls){
	parser_advance(pls);
	
	AstNode *bin = mk_safe_node();
	bin->left = mk_true();
	bin->right = parser_collect_block(pls);
	bin->type = P_ELSE;

	return bin;
}

AstNode* parse_if(Parser* pls){
	// statements
	AstNode* start = NULL;
	AstNode* recent = NULL;

	append_list_node(&start, &recent, parser_if_else(pls));
	skip_whitespace(pls);

	while (pls->tok != NULL && (pls->tok->type == TK_ELIF || pls->tok->type == TK_ELSE || pls->tok->type == TK_NL)){
		if (pls->tok->type == TK_ELSE){
			append_list_node(&start, &recent, parser_else(pls));
			break;
		
		} else if (pls->tok->type == TK_NL){
			parser_advance(pls);

		} else {
			append_list_node(&start, &recent, parser_if_else(pls));
		}
	}

	AstNode* bin = mk_safe_node();
	bin->type = P_IF;
	bin->left = start;

	return bin;
}

AstNode *parse_while(Parser *pls) {
	parser_advance(pls);
	AstNode *bin = mk_safe_node();

	bin->left = parser_eval(pls);
	bin->right = parser_collect_block(pls);
	bin->type = P_WHILE;

	return bin;
}

AstNode *parse_break(Parser *pls){
	AstNode *bin = mk_safe_node();
	bin->type = P_BREAK;
	parser_advance(pls);
	return bin;
}

AstNode *parse_continue(Parser *pls){
	AstNode *bin = mk_safe_node();
	bin->type = P_CONTINUE;
	parser_advance(pls);
	return bin;
}

void parser_tree(Parser* pls) {
    while (pls->tok != NULL) {
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

					case TK_CLASS:
						append_node(pls, parse_class(pls));
						break;

					case TK_WHILE:
						append_node(pls, parse_while(pls));
						break;

					case TK_BREAK:
						append_node(pls, parse_break(pls));
						break;

					case TK_CONTINUE:
						append_node(pls, parse_continue(pls));
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
