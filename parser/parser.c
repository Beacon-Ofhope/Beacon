#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"


Parser* parser_read(Token** toks){
	Parser* pls = calloc(1, sizeof(struct PARSE_UP));
	pls->tok = *toks;
}

void parser_advance(Parser* pls){
	if (pls->tok != NULL)
		pls->tok = pls->tok->next;
}

void syntax_error(char* error, int line){
	fprintf(stderr, "Syntax error: %s\n\tline: %d\n", error, line);
	exit(0);
}

void appendNode(AstNode** head, AstNode* newToken) {
    if (*head == NULL) {
        *head = newToken;
    } else {
        AstNode* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newToken;
    }
}

AstNode* parser_factor(Parser* pls, char* end){
	AstNode* node = (AstNode*)malloc(sizeof(AstNode));

	if (pls->tok->type == TK_NUM){
		node->value.num_value = strtod(pls->tok->value, NULL);
		node->type = P_NUM;

	} else if (pls->tok->type == TK_STR){
		node->value.str_value = pls->tok->value;
		node->type = P_STR;

	} else if (pls->tok->type == TK_ID){
		node->value.str_value = pls->tok->value;
		node->type = P_VAR;

	}
	parser_advance(pls);

	while (pls->tok != NULL && (pls->tok->type == TK_LPAREN || pls->tok->type == TK_LBRACK )){
		if (pls->tok->type == TK_LPAREN){
			AstNode* data = (AstNode*)malloc(sizeof(AstNode));
			data->type = P_FN_CALL;
			data->left = node;
			data->right = parser_get_arguments(pls);
			node = data;
		}
		parser_advance(pls);
	}
	return node;
}

AstNode* parser_mult(Parser* pls, char* end){
	AstNode* left = parser_factor(pls, end);

	while (pls->tok != NULL && pls->tok->type == TK_MULT_DIV){
		char* op = pls->tok->value;
		parser_advance(pls);

		AstNode* right = parser_factor(pls, end);

		AstNode* bin = (AstNode*)malloc(sizeof(AstNode));
		bin->type = P_BIN_OP;
		bin->left = left;
		bin->right = right;
		bin->value.str_value = op;

		left = bin;
	}

	return left;
}

AstNode* parser_add_sub(Parser* pls, char* end){
	AstNode* left = parser_mult(pls, end);

	while (pls->tok != NULL && pls->tok->type == TK_ADD_SUB){
		char* op = pls->tok->value;
		parser_advance(pls);

		AstNode* right = parser_mult(pls, end);

		AstNode* bin = (AstNode*)malloc(sizeof(AstNode));
		bin->type = P_BIN_OP;
		bin->left = left;
		bin->right = right;
		bin->value.str_value = op;

		left = bin;
	}

	return left;
}

AstNode* parser_eval(Parser* pls, char* end){
	AstNode* left = parser_add_sub(pls, end);

	while (pls->tok != NULL && pls->tok->type == TK_GREATER_EQS){
		char* op = pls->tok->value;
		parser_advance(pls);

		AstNode* right = parser_add_sub(pls, end);

		AstNode* bin = (AstNode*)malloc(sizeof(AstNode));
		bin->type = P_BIN_OP;
		bin->left = left;
		bin->right = right;
		bin->value.str_value = op;

		left = bin;
	}

	return left;
}

// make a list for nodes that collect a node list
void append_parser_list(AstNode** head, AstNode* newToken) {
    if (*head == NULL) {
        *head = newToken;
    } else {
        AstNode* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newToken;
    }
}

// collects function arguments e.g print(1,..)
AstNode* parser_get_arguments(Parser* pls){
	parser_advance(pls);
	AstNode* args = NULL;

	while (pls->tok != NULL && pls->tok->type != TK_RPAREN){
		if (pls->tok->type == TK_NL || pls->tok->type == TK_COMMA){
			parser_advance(pls);

		} else {
			if (pls->tok != NULL && pls->tok->type == TK_ID){
				AstNode* default_paramter = parser_eval(pls, ",)");

				if (pls->tok != NULL && pls->tok->type == TK_EQUALS){
					parser_advance(pls);
					char* parameter_name = default_paramter->value.str_value;

					default_paramter = parser_eval(pls, ",)");
					
				} else {
					append_parser_list(&args, default_paramter);
				}

			} else {
				append_parser_list(&args, parser_eval(pls, ",)"));
			}
		}
	}
	return args;
}

// parse shortcut for calling functions, attributes and more.
AstNode* parser_id_call(Parser* pls, char* variable, char* end){
	AstNode* node = (AstNode*)malloc(sizeof(AstNode));

	node->value.str_value = variable;
	node->type = P_VAR;

	while (pls->tok != NULL && (pls->tok->type == TK_LPAREN || pls->tok->type == TK_LBRACK )){
		if (pls->tok->type == TK_LPAREN){
			AstNode* data = (AstNode*)malloc(sizeof(AstNode));
			data->type = P_FN_CALL;
			data->left = node;
			data->right = parser_get_arguments(pls);
			node = data;
		}
	}
	return node;
}

AstNode* parse_variable(Parser* pls){
	char* name = pls->tok->value;
	parser_advance(pls);

	AstNode* ast = calloc(1, sizeof(AstNode));

	if (pls->tok != NULL && pls->tok->type == TK_EQUALS){
		parser_advance(pls);

		if (pls->tok == NULL)
			syntax_error("unexpected syntax when assigning a variable", pls->tok->line);

		ast->type = P_VAR_ASSIGN;
		ast->value.str_value = name;
		ast->left = parser_eval(pls, "\\n");

		// if (pl)
		printf("%d....................****\n", pls->tok->type);

	} else if (pls->tok != NULL && pls->tok->type == TK_LPAREN){
		ast = parser_id_call(pls, name, "\\n");

	} else {
		syntax_error("unexpected syntax when assigning a variable", pls->tok->line);
	}

	return ast;
}

AstNode* parser_get_parameters(Parser* pls){
	AstNode* parameters_head = NULL;

	if (pls->tok->type != TK_LPAREN)
		syntax_error("unexpected syntax after fun name token, expected '('", pls->tok->line);

	parser_advance(pls);
	char no_default_yet = 0;

	while (pls->tok != NULL && pls->tok->type != TK_RPAREN){
		if (pls->tok->type == TK_ID){
			char* parameter = pls->tok->value;
			parser_advance(pls);

			if (pls->tok != NULL && pls->tok->type == TK_EQUALS){
				AstNode* node = (AstNode*)malloc(sizeof(AstNode));
				parser_advance(pls);

				node->type = P_VAR_ASSIGN;
				node->value.str_value = parameter;
				node->left = parser_eval(pls, ",)");
				appendNode(&parameters_head, node);
				no_default_yet = 1;

			} else {
				if (no_default_yet == 1)
					syntax_error("unexpected syntax in fun declaration, non-default parameters come before all default one", pls->tok->line);

				AstNode* node = (AstNode*)malloc(sizeof(AstNode));

				node->value.str_value = parameter;
				node->type = P_VAR;
				appendNode(&parameters_head, node);
			}
		}
		else if (pls->tok->type == TK_COMMA)
			parser_advance(pls);
		else
			syntax_error("unexpected syntax in fun parameters", pls->tok->line);

	}

	parser_advance(pls);
	return parameters_head;
}

void skip_whitespace(Parser* pls){
	while (pls->tok != NULL && pls->tok->type == TK_NL)
		parser_advance(pls);
}

AstNode* parser_collect_block(Parser* pls){
	skip_whitespace(pls);

	if (pls->tok == NULL && pls->tok->type != TK_LBRACE)
		syntax_error("expected '{' token to get code block.", pls->tok->line);

	parser_advance(pls);
	int count = 1;
	Token* block_token_head = NULL;

	while (pls->tok != NULL && count > 0){
		if (pls->tok->type == TK_LBRACE){
			count++;
		} else if (pls->tok->type == TK_RBRACE){
			count--;
		}

		if (count > 0){
			Token* tok = (Token*)malloc(sizeof(Token));
			tok->line = pls->tok->line;
			tok->type = pls->tok->type;
			tok->value = pls->tok->value;

			appendToken(&block_token_head, tok);
			parser_advance(pls);
		}
	}

	parser_advance(pls);

	AstNode* block_parser_head = NULL;
  parser_tree(parser_read(&block_token_head), &block_parser_head);

	return block_parser_head;
}

AstNode* parse_function(Parser* pls){
	parser_advance(pls);

	if (pls->tok->type != TK_ID)
		syntax_error("unexpected syntax after 'fun' token", pls->tok->line);

	AstNode* ast = calloc(1, sizeof(AstNode));
	ast->value.str_value = pls->tok->value;

	parser_advance(pls);

	ast->left = parser_get_parameters(pls);
	ast->right = parser_collect_block(pls);
	ast->type = P_FN;

	return ast;
}

void parser_tree(Parser* pls, AstNode** head){

	while (pls->tok != NULL) {
        if (pls->tok->type == TK_ID){
        	appendNode(head, parse_variable(pls));

		} else if (pls->tok->type == TK_FUN){
			appendNode(head, parse_function(pls));

        } else {
        	parser_advance(pls);
        }
    }
}
