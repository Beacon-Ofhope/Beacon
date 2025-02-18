#ifndef TOKEN_H
#define TOKEN_H

typedef enum
{
    TK_ID = 0,
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
    TK_DOT,
    TK_FUN,
    TK_THROW,
    TK_RETURN,
    TK_CLASS,
    TK_WHILE,
    TK_FOR,
    TK_IMPORT,
    TK_FROM,
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
    TK_TRY,
    TK_CATCH,
    TK_COLON,
    TK_SEMICOLON,
    TK_BREAK,
    TK_FACE,
    TK_CONTINUE,
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
    unsigned short line;
    char *code;
    char *file;
    unsigned int pos;
    unsigned int code_len;

	struct TOKEN* start;
	struct TOKEN* resent;
} Lexer;

Lexer* lexer_read(char * code, char* file);

void lexer_process(Lexer* lex);

#endif
