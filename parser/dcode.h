
#ifndef H_DCODE
#define H_DCODE

#include "parser.h"
#include "dobject.h"


typedef struct DCODE_UP{
	AstNode* tok;
} Inter;


typedef enum {
	I_NUM=0,
	I_STR,
	I_BOOL,
	I_NONE,
	I_LIST,
	I_DICT,
	I_VAR,
	I_VAR_MAKE,
	I_FN,
	I_CALL_FN,
} Dcode_Type;


typedef struct DCODE {
    Dcode_Type type;
    char* name;
    union {
        char* str_value;
        double num_value;
    } value;

	struct DCODE* left;
    struct DCODE* right;
    Dobject* (*func)(struct DCODE*, Stack*);
    struct DCODE* next;
} Dcode;

Inter* Interpo_read(AstNode** toks);

void Interpo_advance(Inter* pls);

void Interpo_start(Inter* pls, Dcode** head);


Dcode* call_function(AstNode* tok);

#endif
