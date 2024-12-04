#ifndef H_DOBJECT
#define H_DOBJECT


typedef enum {
	D_NUM=0,
	D_STR,
	D_FN,
	D_BOOL,
	D_NONE,
	D_LIST,
	D_DICT,
} Dobject_Type;

typedef struct STACK {
    struct entry_t **entries;
} Stack;

typedef struct PARAMETERS{
	char* value;
	struct PARAMETERS *next;
} Param;

typedef struct DOBJECT {
	Dobject_Type type;
	union {
		char* str_value;
		double num_value;
	}value;

	Param* params;
    struct DOBJECT* (*func)(struct DOBJECT*, Stack*);
	struct DOBJECT *default_data;
	struct DOBJECT *next;
} Dobject;

#define TABLE_SIZE 20000

typedef struct entry_t {
    char *key;
    Dobject *value;
    struct entry_t *next;
} entry_t;


unsigned int hash(const char *key);

entry_t *pair_stack(const char *key, Dobject *value);

Stack *create_stack(void);

void add_to_stack(Stack *hashtable, const char *key, Dobject *value);

Dobject* get_from_stack(Stack *hashtable, const char *key);

void delete_from_stack(Stack *hashtable, const char *key);

void print_stack(Stack *hashtable);

Dobject* b_None();


#endif