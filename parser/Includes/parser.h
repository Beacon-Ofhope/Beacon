#ifndef H_PARSER
#define H_PARSER

#include "lexer.h"

typedef enum {
	P_NUM = 0,
	P_STR,
	P_DICTIONARY,
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
	P_TRY,
	P_CATCH,
	P_BREAK,
	P_THROW,
	P_CONTINUE,
	P_IF,
	P_FACE,
	P_ELSE,
	P_NOT,
	P_IF_ELSE,
	P_IMPORT,
	P_IMPORT_INCLUDE,
} AstType;

typedef struct ASTNode {
	unsigned short line;
    AstType type;

    union {
		double number;
		char *name;
		struct _pcall *call;
		struct _pdict *dict;
		struct _pbinop *binop;
		struct _piface *face;
		struct _pgattr *gattr;
		struct _psattr *sattr;
		struct _pmkvar *mkvar;
		struct _pmkfun *mkfun;
		struct _pmkclass *mkclass;
		struct _pwhile **pif;
		struct _ptry *ptry;
		struct _pwhile *pwhile;
		struct _pimport *pimport;
		struct ASTNode *data;
	} value;

    struct ASTNode* next;
} AstNode;

typedef struct _ptry {
	char *error_name;
	AstNode *tried;
	AstNode *retry;
} ptry;

typedef struct _pwhile {
	AstNode *condition;
	AstNode *code_block;
} pwhile;

// resizing array structs e.g if stats
typedef struct _ptrack {
	unsigned char capacity;
	unsigned char count;
	void **pointer;
} ptrack;

typedef struct _pmkclass {
	char *name;
	AstNode *exts;
	AstNode *code_block;
} pmkclass;

typedef struct _pmkfun {
	char *name;
	AstNode *code_block;
	char **params;
} pmkfun;

typedef struct _pmkvar {
	char *key;
	AstNode *value;
} pmkvar;

typedef struct _pgattr {
	char *key;
	AstNode *parent;
} pgattr;

typedef struct _psattr {
	char *key;
	char *operator;
	AstNode *parent;
	AstNode *value;
} psattr;

typedef struct _piface {
	unsigned char count;
	AstNode *values;
	char **keys;
} piface;

typedef struct _pbinop {
	char *operator;
	AstNode *left;
	AstNode *right;
} pbinop;

typedef struct _pcall {
	unsigned char count;
	AstNode *callee;
	AstNode *args;
} pcall;

typedef struct _pdict {
	AstNode *values;
	char **keys;
} pdict;

typedef struct _pport {
	char *name;
	char *alias;
} pport;

typedef struct _pimport {
	unsigned char count;
	union {
		pport **dirs;
		pport *dir;
	} imports;
	char *path;
} pimport;

typedef struct PARSE_UP{
	char *file;
	Token *tok;
	AstNode* start;
	AstNode* recent;
} Parser;

Parser* parser_read(const Lexer* lex);

void parser_tree(Parser* pls);

AstNode* parser_factor(Parser* pls);

AstNode* parser_eval(Parser* pls);

AstNode * parser_add_sub(Parser * pls);

AstNode * parser_mult(Parser * pls);

AstNode *parser_call_function(AstNode *node, Parser *pls);

AstNode *parser_get_attribute(AstNode *node, Parser *pls);

#endif
