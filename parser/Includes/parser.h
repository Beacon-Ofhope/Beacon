
#ifndef H_PARSER
#define H_PARSER

#include "lexer.h"

typedef enum
{
	P_NUM = 0,
	P_STR,
	P_VAR,
	P_NONE,
	P_TRUE,
	P_FALSE,
	P_BIN_OP,
	P_VAR_ASSIGN,
	P_LIST,
	P_FN,
	P_FN_CALL,
	P_RETURN,
	P_ATTR,
	P_SET_ATTR,
	P_CLASS,
	P_FOR,
	P_WHILE,
	P_BREAK,
	P_CONTINUE,
	P_IF,
	P_ELSE,
	P_NOT,
	P_IF_ELSE,
} AstType;

typedef struct ASTNode {
    AstType type;
    union {
        char* str_value;
		double num_value;
    } value;

	unsigned int line;

    struct ASTNode* left;
    struct ASTNode* right;

    struct ASTNode* next;
} AstNode;


typedef struct PARSE_UP{
	Token* tok;
    char* file;
	AstNode* start;
	AstNode* recent;
} Parser;


Parser* parser_read(const Lexer* lex);

void parser_tree(Parser* pls);

AstNode* parser_eval(Parser* pls);

AstNode * parser_add_sub(Parser * pls);

AstNode * parser_mult(Parser * pls);

AstNode *parser_call_function(AstNode *node, Parser *pls);

AstNode *parser_get_attribute(AstNode *node, Parser *pls);

AstNode *parser_set_attribute(AstNode *node, Parser *pls);

#endif
