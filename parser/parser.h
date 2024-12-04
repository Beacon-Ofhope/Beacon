
#ifndef H_PARSER
#define H_PARSER

#include "lexer.h"

typedef enum {
	P_NUM=0,
	P_STR,
    P_VAR,
    P_BIN_OP,
	P_VAR_ASSIGN,
	P_FN,
    P_FN_CALL,
} AstType;

typedef struct ASTNode {
    AstType type;
    union {
        char* str_value;
				double num_value;
    } value;
    struct ASTNode* left;
    struct ASTNode* right;

    struct ASTNode* next;
} AstNode;


typedef struct PARSE_UP{
	Token* tok;
} Parser;


Parser* parser_read(Token** toks);

void parser_advance(Parser* pls);

void appendNode(AstNode** head, AstNode* newToken);

AstNode* parser_factor(Parser* pls, char* end);

AstNode* parser_mult(Parser* pls, char* end);

AstNode* parser_eval(Parser* pls, char* end);

void append_parser_list(AstNode** head, AstNode* newToken);

AstNode* parser_get_arguments(Parser* pls);

AstNode* parser_id_call(Parser* pls, char* variable, char* end);

AstNode* parse_variable(Parser* pls);

void parser_tree(Parser* pls, AstNode** head);



#endif
