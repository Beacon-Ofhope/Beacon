#ifndef H_DOBJECT
#define H_DOBJECT

typedef enum {
	B_NUM = 0,
	B_STR,
	B_FN,
	B_BOOL,
	B_NONE,
	B_LIST,
	B_DICT,
	B_TYPE,
	B_OBJECT,
} Bobject_Type;

typedef struct STACK {
    struct entry_t **entries;
} Stack;

typedef struct PARAMETERS{
	char* value;
	struct PARAMETERS *next;
} Param;

typedef struct garbage_collector {
} bcon_GC;

typedef struct beacon_error {
    char *file;
    char *message;
    unsigned int line;
} bcon_err;

typedef struct beacon_State {
	int islocked;
	bcon_GC *GC;
	Stack *memory;
    bcon_err *errors;
	struct BOBJECT* return_value;
} bcon_State;

typedef struct BOBJECT {
	Bobject_Type type;
	union {
		char* str_value;
		double num_value;
	} value;

	Param* params;
	struct BCODE *code;
	struct STACK *attrs;
	struct BOBJECT *this;
	struct BOBJECT *next;
	struct BOBJECT *left;
	struct BOBJECT *(*func)(struct BOBJECT *, struct BOBJECT *, bcon_State *);
} Bobject;


#define TABLE_SIZE 1000

typedef struct entry_t {
    char *key;
    Bobject *value;
    struct entry_t *next;
} entry_t;


unsigned int hash(const char *key);

entry_t *pair_stack(const char *key, Bobject *value);

Stack *create_stack(void);

void add_to_stack(Stack *hashtable, const char *key, Bobject *value);

Bobject* get_from_stack(Stack *hashtable, const char *key);

void delete_from_stack(Stack *hashtable, const char *key);

void print_stack(Stack *hashtable);

Bobject *b_None();
Bobject *BC_STRING(char *data);

#endif