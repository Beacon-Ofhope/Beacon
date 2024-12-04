#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    TK_ID=0,
    TK_NUM,
    TK_FLOAT,
    TK_STR,
    TK_LPAREN,
    TK_RPAREN,
    TK_LBRACE,
    TK_RBRACE,
    TK_LBRACK,
    TK_RBRACK,
    TK_EQUALS,
    TK_NL,
    TK_COMMA,
    TK_FUN,
    TK_CLASS,
    TK_WHILE,
    TK_FOR,
    TK_IMPORT,
    TK_AS,
    TK_IN,
    TK_NOT,
    TK_TRUE,
    TK_FALSE,
    TK_PASS,
    TK_IF,
    TK_ELIF,
    TK_ELSE,
    TK_NONE,
    TK_COLON,
    TK_SEMICOLON,
    TK_BREAK,
    TK_CONT,
    TK_ADD_SUB,
    TK_MULT_DIV,
    TK_GREATER_EQS,
} TokenType;


typedef struct TOKEN{
	char * value;
	TokenType type;
	unsigned int line;
	struct TOKEN* next;
} Token;


typedef struct LEX_UP{
	char tok;
	char * code;
	unsigned int pos;
	unsigned int line;
	unsigned int code_len;

} Lexer;

Token* init_token(char* value, TokenType type, unsigned int line);

void appendToken(Token** head, Token* newToken);

Lexer* lexer_read(char * code);

void lexer_advance(Lexer* lex);

Token* keyword_id(char* value, Lexer* lex);

Token* lex_id(Lexer* lex);

Token* lex_num(Lexer* lex);

Token* lex_eqs(Lexer* lex);

Token* lex_newline(Lexer* lex);

void lex_special(Lexer* lex, Token** head);

void lexer_process(Lexer* lex, Token** head);


#endif
