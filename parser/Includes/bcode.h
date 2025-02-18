
#ifndef H_DCODE
#define H_DCODE

#include "parser.h"
#include "bobject.h"

typedef enum {
    OP_NUMBER,
    OP_STRING,
    OP_BOOL,
    OP_NONE,
    OP_LIST,
    OP_DICTIONARY,
    OP_VARIABLE,
    OP_MAKE_VARIABLE,
    OP_CLASS,
    OP_RETURN,
    OP_FUNCTION,
    OP_FUNCTION_CALL,
    OP_GET_ATTRIBUTE,
    OP_SET_ATTRIBUTE,
    OP_IF,
    OP_WHILE,
    OP_BREAK,
    OP_CONTINUE,
    OP_TRY_CATCH,
    OP_THROW,
    OP_FACE,
    OP_NOT,
    OP_IMPORT,
    OP_IMPORT_INCLUDE,
    OP_BINARY_OPERATION,
} OP_CODE;

typedef struct BCODE {
    OP_CODE type;
    unsigned short line;

    union {
        void *d;
        char *name;
        double number;
		struct _opbinop *binop;
        struct _opdict *dict;
        struct _opgattr *pget;
        struct _opsattr *pset;
        struct _opcall *call;
        struct _optry *ptry;
        struct _opmkvar *mkvar;
        struct _opmkfun *pmkfun;
        struct _opmkclass *pmkclass;
        struct _opwhile *pwhile;
        struct BCODE *data;
        struct _opwhile **pif;
        struct _pimport *pimport;
    } value;

    Bobject *(*func)(struct BCODE *, bcon_State *);
    struct BCODE* next;
} Bcode;

typedef struct _opbinop {
	Bcode *left;
	Bcode *right;
} opbinop;

typedef struct _opdict {
	Bcode *values;
	char **keys;
} opdict;

typedef struct _opgattr {
	char *key;
	Bcode *parent;
} opgattr;

typedef struct _opsattr {
	char *key;
	char *operator;
	Bcode *parent;
	Bcode *value;
} opsattr;

typedef struct _optry {
    char *error_name;
    Bcode *tried;
    Bcode *retry;
} optry;

typedef struct _opmkclass {
	char *name;
	Bcode *exts;
	Bcode *code_block;
} opmkclass;

typedef struct _opmkfun {
	char *name;
	Bcode *code_block;
	char **params;
} opmkfun;

typedef struct _opwhile {
	Bcode *condition;
	Bcode *code_block;
} opwhile;

typedef struct _opmkvar {
	char *key;
	Bcode *value;
} opmkvar;

typedef struct _opcall {
	unsigned char count;
    Bcode *args;
    Bcode *callee;
    bargs *argv;
} opcall;

typedef struct BCODE_UP {
	AstNode * tok;
    char * file;
    Bcode * start;
    Bcode * recent;
} Inter;

Inter * inter_read(const Parser * pls);

void inter_interpret(Inter * pls);

Bcode *i_code_block(Inter *pls, AstNode *ast);

Bcode *i_eval_ast(AstNode *value, Inter *pls);

Bcode *i_call_function(AstNode * tok, Inter * pls);

#endif
