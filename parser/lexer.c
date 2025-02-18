#include "Includes/lexer.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


Token * init_token(char* value, TokenType type, unsigned short line){
	Token* token = malloc(sizeof(Token));
	token->value = value;
	token->type = type;
	token->line = line;
	token->next = NULL;

    return token;
}

void lexer_syntax_error(char* error, const Lexer* lex){
    printf("  File '%s' at line %d\n \n", lex->file, lex->line);
    printf("SyntaxError: %s\n", error);
    exit(0);
}

void appendToken(Token* newToken, Lexer* lex) {
	if (lex->resent == NULL){
        lex->start = newToken;
  		lex->resent = newToken;
    } else {
    	lex->resent->next = newToken;
        lex->resent = newToken;
    }
}

Lexer* lexer_read(char * code, char* file){
	Lexer* lex = calloc(1, sizeof(struct LEX_UP));
	lex->resent = NULL;
    lex->start = NULL;

	lex->file = file;
	lex->code = code;

	lex->line = 1;
	lex->code_len = strlen(code);

	lex->pos = 0;
	lex->tok = code[lex->pos];
	return lex;
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

Token* keyword_id(char* value, const Lexer* lex){
    if (strcmp("fun", value) == 0)
        return init_token(NULL, TK_FUN, lex->line);

    if (strcmp("return", value) == 0)
        return init_token(NULL, TK_RETURN, lex->line);

    if (strcmp("class", value) == 0)
        return init_token(NULL, TK_CLASS, lex->line);

    if (strcmp("from", value) == 0)
        return init_token(NULL, TK_FROM, lex->line);

    if (strcmp("as", value) == 0)
        return init_token(NULL, TK_AS, lex->line);

    if (strcmp("import", value) == 0)
        return init_token(NULL, TK_IMPORT, lex->line);

    if (strcmp("while", value) == 0)
        return init_token(NULL, TK_WHILE, lex->line);

    if (strcmp("for", value) == 0)
        return init_token(NULL, TK_FOR, lex->line);

    if (strcmp("in", value) == 0)
        return init_token(NULL, TK_IN, lex->line);

    if (strcmp("not", value) == 0)
        return init_token(NULL, TK_NOT, lex->line);

    if (strcmp("pass", value) == 0)
        return init_token(NULL, TK_PASS, lex->line);

    if (strcmp("break", value) == 0)
        return init_token(NULL, TK_BREAK, lex->line);

    if (strcmp("continue", value) == 0)
        return init_token(NULL, TK_CONTINUE, lex->line);

    if (strcmp("if", value) == 0)
        return init_token(NULL, TK_IF, lex->line);

    if (strcmp("elif", value) == 0)
        return init_token(NULL, TK_ELIF, lex->line);

    if (strcmp("else", value) == 0)
        return init_token(NULL, TK_ELSE, lex->line);

    if (strcmp("try", value) == 0)
        return init_token(NULL, TK_TRY, lex->line);

    if (strcmp("catch", value) == 0)
        return init_token(NULL, TK_CATCH, lex->line);

    if (strcmp("interface", value) == 0)
        return init_token(NULL, TK_FACE, lex->line);

    if (strcmp("throw", value) == 0)
        return init_token(NULL, TK_THROW, lex->line);

    return init_token(value, TK_ID, lex->line);
}

Token* lex_id(Lexer* lex) {
    char* value = malloc(2);

    if (value == NULL){
        lexer_syntax_error("lexer string memory allocation failed...", lex);
    }

    int capacity = 2;
    int len = 0;

    while (lex->tok != '\0' && (isalnum(lex->tok) !=0 || lex->tok == '_')) {
        if ((len+1) == capacity) {
            capacity += 1;
            char* temp = realloc(value, capacity);

            if (temp == NULL) {
                free(value);
                lexer_syntax_error("memory allocation for string failed", lex);
            }
            value = temp;
        }

        value[len] = lex->tok;
        len++;
        lexer_advance(lex);
    }

    value[len] = '\0';
    return keyword_id(value, lex);
}

Token* lex_num(Lexer* lex) {
    char* value = malloc(2);
    int capacity = 2;
    int len = 0;
    int count_dots = 0;

    while (lex->tok != '\0' && (isdigit(lex->tok) !=0 || lex->tok == '_' || lex->tok == '.')) {
   		if (lex->tok == '_'){
            lexer_advance(lex);
            continue;

        } else if (lex->tok == '.'){
            if (count_dots)
                lexer_syntax_error("Invalid syntax, found many dots in number", lex);

            count_dots = 1;
        }

        if ((len+1) == capacity) {
            capacity += 1;
            value = realloc(value, capacity);
        }

        value[len] = lex->tok;
        len++;
        lexer_advance(lex);
    }

    value[len] = '\0';
    return init_token(value, TK_NUM, lex->line);
}

Token* lex_string(Lexer* lex) {
    const char quot = lex->tok;
    lexer_advance(lex);

    char* value = malloc(100);
    int len = 0;
    int capacity = 100;

    while (lex->tok != '\0' && lex->tok != quot && lex->tok != '\n') {
        if (len == capacity-1) {
            capacity += 100;
            value = (char*)realloc(value, capacity);
        }

        if (lex->tok != '\\')
            value[len] = lex->tok;
        
        else {
            lexer_advance(lex);

            if (lex->tok == '\0')
                break;
            
            switch (lex->tok) {
                case 'a':
                    value[len] = '\a';
                    break;
                case 'b':
                    value[len] = '\b';
                    break;
                case 'f':
                    value[len] = '\f';
                    break;
                case 'n':
                    value[len] = '\n';
                    break;
                case 'r':
                    value[len] = '\r';
                    break;
                case 't':
                    value[len] = '\t';
                    break;
                case 'v':
                    value[len] = '\v';
                    break;
                case '\\':
                    value[len] = '\\';
                    break;
                case '"':
                    value[len] = '"';
                    break;
                case '\'':
                    value[len] = '\'';
                    break;
                case '\n':
                    len--;
                    break;
                default:
                    value[len] = lex->tok;
                    break;
            }
        }
        len++;
        lexer_advance(lex);
    }

    switch (lex->tok){
        case '\0':
    	    lexer_syntax_error("reached EOF before string termination.", lex);
            break;
        case '\n':
            lexer_syntax_error("Found newline before string untermination.", lex);
            break;
        default:
            value = realloc(value, len + 1);
            lexer_advance(lex);

            value[len] = '\0';
            break;
    }

    // printf("%s\n", value);
    
    return init_token(value, TK_STR, lex->line);
}

Token* lex_newline(Lexer* lex, int determine){
	lexer_advance(lex);

    if (determine)
	    lex->line++;

    return init_token(NULL, TK_NL, lex->line);
}

Token* lex_operator(Lexer* lex){
    char* value = malloc(2);
	value[0] = lex->tok;
	value[1] = '\0';

	lexer_advance(lex);

	if (strcmp("*", value) == 0 || strcmp("/", value) == 0 || strcmp("%", value) == 0)
    	return init_token(value, TK_MULT_DIV, lex->line);

   	return init_token(value, TK_ADD_SUB, lex->line);
}

void lex_special(Lexer* lex){
	switch (lex->tok){
		case '(':
			return appendToken(init_token(NULL, TK_LPAREN, lex->line), lex);
		case ')':
			return appendToken(init_token(NULL, TK_RPAREN, lex->line), lex);
		case ']':
			return appendToken(init_token(NULL, TK_RBRACK, lex->line), lex);
		case '[':
			return appendToken(init_token(NULL, TK_LBRACK, lex->line), lex);
		case '}':
			return appendToken(init_token(NULL, TK_RBRACE, lex->line), lex);
		case '{':
			return appendToken(init_token(NULL, TK_LBRACE, lex->line), lex);
		case ',':
			return appendToken(init_token(NULL, TK_COMMA, lex->line), lex);
        case '.':
            return appendToken(init_token(NULL, TK_DOT, lex->line), lex);
        case ';':
			return appendToken(init_token(NULL, TK_SEMICOLON, lex->line), lex);
        case ':':
            return appendToken(init_token(NULL, TK_COLON, lex->line), lex);
        default:
            if (isspace(lex->tok) == 0){
				lexer_syntax_error("Unexpected syntax in code", lex);
            }
            break;
	}
}

void skip_comment(Lexer* lex){
	lexer_advance(lex);

	if (lex->tok == '/'){
		while (lex->tok != '\0' && lex->tok != '\n')
			lexer_advance(lex);
	} else {
        appendToken(init_token("/", TK_MULT_DIV, lex->line), lex);
    }
}

Token* lex_conditions(Lexer* lex){
    const char op = lex->tok;
	lexer_advance(lex);

    if (lex->tok != '\0' && lex->tok == '='){
		lexer_advance(lex);
		char* value1 = malloc(3);
        value1[0] = op; value1[1] = '='; value1[2] = '\0';

		return init_token(value1, TK_GREATER_EQS, lex->line);
	}

	char* value2 = malloc(2);
    value2[0] = op; value2[1] = '\0';

    switch (op) {
        case '=':
            return init_token(value2, TK_EQUALS, lex->line);
        case '!':
            return init_token(value2, TK_NOT, lex->line);
        default:
            break;
    }

    return init_token(value2, TK_GREATER_EQS, lex->line);
}

void lexer_process(Lexer* lex){
	while (lex->tok != '\0'){
        switch (lex->tok) {
            case '\'':
            case '"':
                appendToken(lex_string(lex), lex);
                break;
            case '\n':
                appendToken(lex_newline(lex, 1), lex);
                break;
            case ';':
                appendToken(lex_newline(lex, 0), lex);
                break;
            case '/':
                skip_comment(lex);
                break;
            default:
                if (isalpha(lex->tok) != 0 || lex->tok == '_')
                    appendToken(lex_id(lex), lex);

                else if (isdigit(lex->tok) != 0)
                    appendToken(lex_num(lex), lex);

                else if (strchr("<>!=", lex->tok) != NULL)
                    appendToken(lex_conditions(lex), lex);

                else if (strchr("+-/*%", lex->tok) != NULL)
                    appendToken(lex_operator(lex), lex);

                else {
                    lex_special(lex);
                    lexer_advance(lex);
                }
                break;
        }
	}
}
