#ifndef H_DOBJECT
#define H_DOBJECT

#define BERRNO -808

#include <stdbool.h>

typedef enum
{
	BDICT,
	BNONE,
	BLIST,
	BTYPE,
	BNUMBER,
	BOBJECT,
	BMODULE,
	BSTRING,
	BFUNCTION,
	BFUNCTION2,
	BINTERFACE,
} bdatatype;

typedef struct PARAMETERS{
	char* value;
	struct PARAMETERS *next;
} Param;

typedef struct beacon_State {
	int islocked;
	int stackPos;
	int stackCapacity;
	struct STACK **callStack;
	char *file;
	char *base_folder;
	struct BOBJECT *none;
	struct BOBJECT *return_value;
	struct STACK *memory;
	struct STACK *modules;
} bcon_State;

typedef struct BOBJECT {
	void *d;
	int refs;
	bdatatype type;
	union {
		char* str_value;
		double num_value;
		struct _bmodule *module;
		struct _bfunction *bfun;
		struct _bfunction2 *bfun2;
		struct _btype *btype;
		struct _bclass *bclass;
		struct STACK *attrs;
		struct _blist *list;
		struct _binterface *bface;
	} value;
} Bobject;

typedef struct _bargs {
	unsigned char count;
	unsigned short line;
	Bobject** argv;
} bargs;

typedef struct _bmodule {
	char *name;
	struct STACK *attrs;
} bmodule;

typedef struct _binterface {
	unsigned char count;
	struct STACK *attrs;
} binterface;

typedef struct _bfunction {
	unsigned char count;
	char *name;
	char **params;
	Bobject *this;
	struct BCODE *code;
	struct beacon_State *bstate;
	Bobject *(*func)(bargs *, struct _bfunction *, bcon_State *);
} bfunction;

typedef struct _bfunction2 {
	void *ptr;
	char *name;
	Bobject *(*func)(bargs *, struct _bfunction2 *, bcon_State *);
} bfunction2;

typedef struct _btype {
	char *name;
	struct BCODE *code;
	struct STACK *attrs;
	struct BOBJECT *extends;
	struct beacon_State *bstate;
	Bobject *(*func)(bargs *, struct _btype *, bcon_State *);
} btype;

typedef struct _bclass {
	char *name;
	struct STACK *attrs;
} bclass;

typedef struct _blist {
	size_t count;
	size_t capacity;
	Bobject** values;
} blist;

typedef struct NODE {
    char* key;
    Bobject* value;
    struct NODE* next;
} Node;

typedef struct STACK {
	int resize;
	size_t size;
	size_t capacity;
	Node** table;
} Stack;


// Create a new hash table
Stack *create_stack(int size);

Stack* create_callStack(int size);

// Find the entry in the table
Node *find_entry(Stack *ht, const char *key);

// Free an object out of memory
void free_object(Bobject* data);

// Set a value in the hash table
void add_to_stack(Stack *ht, const char *key, Bobject *value);

// Set a value to a non resizing hashtable e.g that stores modules functions
void add_to_stack_i(Stack* ht, const char* key, Bobject* value);

// Get a value from the hash table
Bobject *get_from_stack(Stack *ht, const char *key);

Node *get_from_stack_i(Stack *ht, const char *key);

// Check if a key exists in the hash table
bool hash_table_exists(Stack *ht, const char *key);

// Resize the hash table
void hash_table_resize(Stack *ht, size_t new_capacity);

// Copy the hash table (for new scope)
Stack *copy_stack(Stack *ht);

// Delete the hash table
void delete_stack(Stack *ht);

// Free the hash table
void hash_table_free(Stack *ht);

// Print the hash table
void print_stack(Stack *ht);

Bobject* mk_error(char* type, char* message, char* file, unsigned int line, int err);

Bobject *mk_Bobject(bcon_State *bstate);

#endif