#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "../Include/helpers.h"


Parser* parser_read(const Lexer* lex){
    Parser* pls = malloc(sizeof(Parser));
	pls->tok = lex->start;
    pls->file = lex->file;
    pls->start = NULL;
    pls->recent = NULL;

    return pls;
}

AstNode* mk_safe_node(){
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
AstNode* parser_get_arguments(Parser * pls){
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
                AstNode * def_param = parser_eval(pls);

                if (pls->tok != NULL && pls->tok->type == TK_EQUALS){
                    if(def_param->type != P_VAR){
						free(def_param);
                        parser_syntax_error("Invalid syntax in function arguments", pls);
                    }

                    parser_advance(pls);
                    AstNode* mk_def_var = mk_safe_node();
                    mk_def_var->type = P_VAR_ASSIGN;
					mk_def_var->value.str_value = copy_string_safely(def_param->value.str_value);
                    mk_def_var->left = parser_eval(pls);

					free(def_param);
					append_list_node(&args, &recent, mk_def_var);

                } else {
                    append_list_node(&args, &recent, def_param);
                }
			} else {
				append_list_node(&args, &recent, parser_eval(pls));
			}
		}
	}

    if (pls->tok == NULL) {
        parser_syntax_error("invalid syntax, missing closing ')' token in fun call.", pls);
    }
	return args;
}

// parse shortcut for calling functions, attributes and more.
AstNode* parser_id_call(Parser* pls, char * variable){
	AstNode* node = mk_safe_node();
	node->value.str_value = copy_string_safely(variable);
	node->type = P_VAR;

	while (pls->tok != NULL && (pls->tok->type == TK_LPAREN || pls->tok->type == TK_LBRACK )){
		if (pls->tok->type == TK_LPAREN){
		    AstNode* data = mk_safe_node();
			data->type = P_FN_CALL;
			data->left = node;
			data->right = parser_get_arguments(pls);
			node = data;
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

AstNode* parser_factor(Parser* pls){
	AstNode* node = mk_safe_node();

    if (pls->tok == NULL || node == NULL){
    	parser_syntax_error("parsing issue, data cannot be formed.", pls);
    }

	if (pls->tok->type == TK_NUM) {
        node->value.num_value = strtod(pls->tok->value, NULL);
	    free(pls->tok->value);
		node->type = P_NUM;

	} else if (pls->tok->type == TK_STR){
        clean_token_string(pls, node);
		node->type = P_STR;

	} else if (pls->tok->type == TK_ID){
        clean_token_string(pls, node);
		node->type = P_VAR;

	} else if (pls->tok->type == TK_LBRACK){
		free(node);
		node = mk_list(pls);
	
	} else if (pls->tok->type == TK_LPAREN){
		free(node);
		parser_advance(pls);
		node = parser_eval(pls);

	} else {
		parser_syntax_error("Invalid syntax in expression", pls);

	}
	parser_advance(pls);

	while (pls->tok != NULL && (pls->tok->type == TK_LPAREN || pls->tok->type == TK_LBRACK )){
		if (pls->tok->type == TK_LPAREN){
			AstNode* data = mk_safe_node();
			data->type = P_FN_CALL;
			data->left = node;
			data->right = parser_get_arguments(pls);
			node = data;
		}
		parser_advance(pls);
	}
	return node;
}

AstNode* parser_mult(Parser* pls){
	AstNode* left = parser_factor(pls);

	while (pls->tok != NULL && pls->tok->type == TK_MULT_DIV){
		char* op = pls->tok->value;
		parser_advance(pls);

		AstNode* bin = mk_safe_node();
		bin->type = P_BIN_OP;
		bin->left = left;
		bin->right = parser_factor(pls);
		bin->value.str_value = op;

		left = bin;
	}
	return left;
}

AstNode* parser_add_sub(Parser* pls){
	AstNode* left = parser_mult(pls);

	while (pls->tok != NULL && pls->tok->type == TK_ADD_SUB){
		char* op = pls->tok->value;
		parser_advance(pls);

		AstNode* bin = mk_safe_node();
		bin->type = P_BIN_OP;
		bin->left = left;
		bin->right = parser_mult(pls);
		bin->value.str_value = op;

		left = bin;
	}
	return left;
}

AstNode* parser_eval(Parser* pls){
	AstNode* left = parser_add_sub(pls);

	while (pls->tok != NULL && pls->tok->type == TK_GREATER_EQS){
		char* op = pls->tok->value;
		parser_advance(pls);

		AstNode* bin = mk_safe_node();
		bin->type = P_BIN_OP;
		bin->left = left;
		bin->right = parser_add_sub(pls);
		bin->value.str_value = op;

		left = bin;
	}
	return left;
}

AstNode* parse_variable(Parser* pls){
    char * name = copy_string_safely(pls->tok->value);
	parser_advance(pls);

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

	} else if (pls->tok != NULL && pls->tok->type == TK_LPAREN){
		return parser_id_call(pls, name);

	} else {
		parser_syntax_error("unexpected syntax when assigning a variable", pls);
	}

	parser_syntax_error("unexpected syntax when assigning data to a variable", pls);
	return NULL;
}
//
//AstNode* parser_get_parameters(Parser* pls){
//	AstNode* parameters_head = NULL;
//
//	if (pls->tok->type != TK_LPAREN)
//		syntax_error("unexpected syntax after fun name token, expected '('", pls->tok->line);
//
//	parser_advance(pls);
//	char no_default_yet = 0;
//
//	while (pls->tok != NULL && pls->tok->type != TK_RPAREN){
//		if (pls->tok->type == TK_ID){
//			char* parameter = pls->tok->value;
//			parser_advance(pls);
//
//			if (pls->tok != NULL && pls->tok->type == TK_EQUALS){
//				AstNode* node = (AstNode*)malloc(sizeof(AstNode));
//				parser_advance(pls);
//
//				node->type = P_VAR_ASSIGN;
//				node->value.str_value = parameter;
//				node->left = parser_eval(pls, ",)");
//				appendNode(&parameters_head, node);
//				no_default_yet = 1;
//
//			} else {
//				if (no_default_yet == 1)
//					syntax_error("unexpected syntax in fun declaration, non-default parameters come before all default one", pls->tok->line);
//
//				AstNode* node = (AstNode*)malloc(sizeof(AstNode));
//
//				node->value.str_value = parameter;
//				node->type = P_VAR;
//				appendNode(&parameters_head, node);
//			}
//		}
//		else if (pls->tok->type == TK_COMMA)
//			parser_advance(pls);
//		else
//			syntax_error("unexpected syntax in fun parameters", pls->tok->line);
//
//	}
//
//	parser_advance(pls);
//	return parameters_head;
//}

void skip_whitespace(Parser* pls){
	while (pls->tok != NULL && pls->tok->type == TK_NL)
		parser_advance(pls);
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

AstNode* parser_collect_block(Parser* pls){
	skip_whitespace(pls);

	if (pls->tok == NULL && pls->tok->type != TK_LBRACE)
		parser_syntax_error("expected '{' token to get code block.", pls);

	parser_advance(pls);
	int count = 1;

	// TOKENS
	Token *start = NULL;
	Token *recent = NULL;

	while (pls->tok != NULL && count > 0){
		if (pls->tok->type == TK_LBRACE){
			count++;
		} else if (pls->tok->type == TK_RBRACE){
			count--;
		}

		if (count > 0){
			Token* tok = malloc(sizeof(Token));
			tok->line = pls->tok->line;
			tok->type = pls->tok->type;
			tok->value = pls->tok->value;
			tok->next = NULL;

			parser_append_block_token(&start, &recent, tok);
			parser_advance(pls);
		}
	}

	if (pls->tok == NULL)
		parser_syntax_error("Expected a closing '}' but reached EOF", pls);

	parser_advance(pls);

	Lexer *lex = calloc(1, sizeof(struct LEX_UP));
	lex->start = start;
	lex->file = pls->file;

	Parser *par = parser_read(lex);
	parser_tree(par);

	return par->start;
}

//AstNode* parse_function(Parser* pls){
//	parser_advance(pls);
//
//	if (pls->tok->type != TK_ID)
//		syntax_error("unexpected syntax after 'fun' token", pls->tok->line);
//
//	AstNode* ast = calloc(1, sizeof(AstNode));
//	ast->value.str_value = pls->tok->value;
//
//	parser_advance(pls);
//
//	ast->left = parser_get_parameters(pls);
//	ast->right = parser_collect_block(pls);
//	ast->type = P_FN;
//
//	return ast;
//}

AstNode* parser_if_else(Parser* pls){
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

void parser_tree(Parser* pls) {
    while (pls->tok != NULL) {
		// printf("%d...\n", pls->tok->type);
        if (pls->tok->type == TK_ID) {
            append_node(pls, parse_variable(pls));

		} else if (pls->tok->type == TK_IF){
			append_node(pls, parse_if(pls));

		} else if (pls->tok->type == TK_NL){
			parser_advance(pls);

		} else {
			append_node(pls, parser_eval(pls));
			parser_advance(pls);
		}
	}
}
