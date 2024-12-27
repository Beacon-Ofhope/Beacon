
#ifndef H_DCODE
#define H_DCODE

#include "parser.h"
#include "bobject.h"

typedef enum
{
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
    OP_NOT,
    OP_BINARY_OPERATION,
} OP_CODE;

typedef struct BCODE {
    OP_CODE type;
    char* name;
    unsigned int line;
    union {
        char* str_value;
        double num_value;
    } value;

	struct BCODE* left;
    struct BCODE* right;
    Bobject* (*func)(struct BCODE*, bcon_State*);
    struct BCODE* next;
} Bcode;

typedef struct BCODE_UP{
	AstNode * tok;
    char * file;
    Bcode * start;
    Bcode * recent;
} Inter;

Inter * inter_read(const Parser * pls);

void inter_interpret(Inter * pls);

Bcode *i_eval_ast(AstNode *value, Inter *pls);

Bcode *i_call_function(AstNode * tok, Inter * pls);

#endif
