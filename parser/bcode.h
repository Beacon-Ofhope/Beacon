
#ifndef H_DCODE
#define H_DCODE

#include "parser.h"
#include "bobject.h"

typedef enum
{
    I_NUM = 0,
    I_STR,
    I_BOOL,
    I_NONE,
    I_LIST,
    I_DICT,
    I_VAR,
    I_VAR_MAKE,
    I_CLASS,
    I_FN,
    I_CALL_FN,
    I_ATTR,
    I_IF,
    I_WHILE,
} Bcode_Type;

typedef struct BCODE {
    Bcode_Type type;
    char* name;
    union {
        char* str_value;
        double num_value;
    } value;

	struct BCODE* left;
    struct BCODE* right;
    Bobject* (*func)(struct BCODE*, Stack*);
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
