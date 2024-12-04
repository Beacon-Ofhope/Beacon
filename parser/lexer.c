#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "lexer.h"

Token* init_token(char* value, TokenType type, unsigned int line){
	Token* token = calloc(1, sizeof(Token));
	token->value = value;
	token->type = type;
	token->line = line;
	token->next = NULL;
}

void appendToken(Token** head, Token* newToken) {
    if (*head == NULL) {
        *head = newToken;
    } else {
        Token* current = *head;
        while (current->next != NULL) {
            current = current->next;
				}
        current->next = newToken;
    }
}

Lexer* lexer_read(char * code){
	Lexer* lex = calloc(1, sizeof(struct LEX_UP));

	lex->line = 1;
	lex->code = code;
	lex->code_len = strlen(code);

	lex->pos = 0;
	lex->tok = code[lex->pos];
}

void lexer_advance(Lexer* lex){
	if (lex->pos < lex->code_len && lex->tok != '\0'){
		lex->pos++;
		lex->tok = lex->code[lex->pos];
	}
}

void lexer_back(Lexer* lex){
	if (lex->pos < lex->code_len && lex->tok != '\0'){
		lex->pos--;
		lex->tok = lex->code[lex->pos];
	}
}

Token* keyword_id(char* value, Lexer* lex){
	if (strcmp("fun", value) == 0){
    	return init_token("", TK_FUN, lex->line);

	} else if (strcmp("as", value) == 0){
    	return init_token("", TK_AS, lex->line);

	} else if (strcmp("class", value) == 0){
    	return init_token("", TK_FUN, lex->line);

	} else if (strcmp("import", value) == 0){
    	return init_token("", TK_IMPORT, lex->line);

	} else if (strcmp("while", value) == 0){
    	return init_token("", TK_WHILE, lex->line);

	} else if (strcmp("for", value) == 0){
    	return init_token("", TK_FOR, lex->line);

	} else if (strcmp("in", value) == 0){
    	return init_token("", TK_IN, lex->line);

	} else if (strcmp("not", value) == 0){
    	return init_token("", TK_NOT, lex->line);

	} else if (strcmp("True", value) == 0){
    	return init_token("", TK_TRUE, lex->line);

	} else if (strcmp("False", value) == 0){
    	return init_token("", TK_FALSE, lex->line);

	} else if (strcmp("None", value) == 0){
    	return init_token("", TK_NONE, lex->line);

	} else if (strcmp("pass", value) == 0){
    	return init_token("", TK_PASS, lex->line);

	} else if (strcmp("break", value) == 0){
    	return init_token("", TK_BREAK, lex->line);

	} else if (strcmp("continue", value) == 0){
    	return init_token("", TK_CONT, lex->line);

	} else if (strcmp("if", value) == 0){
    	return init_token("", TK_IF, lex->line);

	} else if (strcmp("elif", value) == 0){
    	return init_token("", TK_ELIF, lex->line);

	} else if (strcmp("else", value) == 0){
    	return init_token("", TK_ELSE, lex->line);
	}

	return init_token(value, TK_ID, lex->line);
}

Token* lex_id(Lexer* lex) {
    char* value = (char *)malloc(1);
    int capacity = 1;
    int len = 0;

    while (lex->tok != '\0' && (isalnum(lex->tok) !=0 || lex->tok == '_')) {
        if (len == capacity) {
            capacity *= 2;
            value = (char *)realloc(value, capacity);
        }

        value[len++] = lex->tok;
        lexer_advance(lex);
    }

    value[len] = '\0';
    return keyword_id(value, lex);
}

Token* lex_num(Lexer* lex) {
    char* value = (char *)malloc(1);
    int capacity = 1;
    int len = 0;

    while (isdigit(lex->tok) !=0 ) {
        if (len == capacity) {
            capacity *= 2;
            value = (char *)realloc(value, capacity);
        }

        value[len++] = lex->tok;
        lexer_advance(lex);
    }

    value[len] = '\0';
    return init_token(value, TK_NUM, lex->line);
}

Token* lex_string(Lexer* lex) {
		char qout = lex->tok;
		lexer_advance(lex);

    char* value = (char *)malloc(1);
    int capacity = 1;
    int len = 0;

    while (lex->tok != '\0' && lex->tok != qout ) {
        if (len == capacity) {
            capacity *= 2;
            value = (char *)realloc(value, capacity);
        }

        value[len++] = lex->tok;
        lexer_advance(lex);
    }

		lexer_advance(lex);
    value[len] = '\0';

    return init_token(value, TK_STR, lex->line);
}

Token* lex_eqs(Lexer* lex){
	lexer_advance(lex);
    return init_token("=", TK_EQUALS, lex->line);
}

Token* lex_newline(Lexer* lex){
	lexer_advance(lex);
	lex->line++;

    return init_token("\\n", TK_NL, lex->line);
}

Token* lex_operator(Lexer* lex){
    char* value = (char *)malloc(1);
	value[0] = lex->tok;
	value[1] = '\0';

	lexer_advance(lex);


	if (strcmp("*", value) == 0 || strcmp("/", value) == 0)
    	return init_token(value, TK_MULT_DIV, lex->line);

   	return init_token(value, TK_ADD_SUB, lex->line);
}

void lex_special(Lexer* lex, Token** head){
	switch (lex->tok){
		case '(':
			return appendToken(head, init_token("", TK_LPAREN, lex->line));
		case ')':
			return appendToken(head, init_token("", TK_RPAREN, lex->line));
		case ']':
			return appendToken(head, init_token("", TK_RBRACK, lex->line));
		case '[':
			return appendToken(head, init_token("", TK_LBRACK, lex->line));
		case '}':
			return appendToken(head, init_token("", TK_RBRACE, lex->line));
		case '{':
			return appendToken(head, init_token("", TK_LBRACE, lex->line));
		case ',':
			return appendToken(head, init_token("", TK_COMMA, lex->line));
		case ';':
			return appendToken(head, init_token("", TK_SEMICOLON, lex->line));
		default:
			break;
	}
}

void skip_comment(Lexer* lex){
	lexer_advance(lex);

	if (lex->tok == '/'){
		while (lex->tok != '\0' && lex->tok != '\n'){
			lexer_advance(lex);
		}
	}
}

Token* lex_conditions(Lexer* lex){
    char op = lex->tok;
	lexer_advance(lex);

	if (lex->tok != '\0' && lex->tok == '='){
		lexer_advance(lex);
		char value1[3] = {op, '=', '\0'};
		return init_token(value1, TK_GREATER_EQS, lex->line);
	}

	char value2[2] = {op, '\0'};
	return init_token(value2, TK_GREATER_EQS, lex->line);
}


void lexer_process(Lexer* lex, Token** head){
	while (lex->tok != '\0'){
		if (isalpha(lex->tok ) != 0 || lex->tok == '_'){
			appendToken(head, lex_id(lex));
		}
		else if (isdigit(lex->tok) !=0){
			appendToken(head, lex_num(lex));
		}
		else if (lex->tok == '\'' || lex->tok == '"'){
			appendToken(head, lex_string(lex));
		}
		else if (lex->tok == '='){
			appendToken(head, lex_eqs(lex));
		}
		else if (lex->tok == '\n'){
			appendToken(head, lex_newline(lex));
		}
		else if (lex->tok == '/') {
			skip_comment(lex);
		}
		else if (strchr("<>!", lex->tok) != NULL) {
			appendToken(head, lex_conditions(lex));
		}
		else if (strchr("+-/*", lex->tok) != NULL){
			appendToken(head, lex_operator(lex));
		}
		else{
			lex_special(lex, head);
			lexer_advance(lex);
		}
	}
}
