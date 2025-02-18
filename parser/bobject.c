#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "Includes/bobject.h"
#include "../Modules/Includes/_modules.h"


Bobject* mk_Bobject(bcon_State* bstate){
    Bobject* obj = malloc(sizeof(Bobject));
    obj->refs = 0;
    return obj;
}

Bobject* mk_error(char* type, char* message, char* file, unsigned int line, int err){
    Stack *attrs = create_stack(5);

    add_to_stack(attrs, "line", mk_mod_number(line));
    add_to_stack(attrs, "file", mk_mod_string(strdup(file)));
    add_to_stack(attrs, "type", mk_mod_string(type));
    add_to_stack(attrs, "message", mk_mod_string(message));
    add_to_stack(attrs, "errno", mk_mod_number(err));

    binterface *in_face = malloc(sizeof(binterface));
    in_face->attrs = attrs;
    in_face->count = 4;

    Bobject *bin = malloc(sizeof(Bobject));
    bin->type = BINTERFACE;
    bin->value.bface = in_face;
    bin->refs = 0;

    return bin;
}

// Hash function (e.g., FNV-1a)
size_t hash(const char* key) {
    size_t hash = 14695981039346656037UL; // 64-bit FNV-1a offset basis
    while (*key) {
        hash ^= *(key++);
        hash *= 1099511628211UL; // 64-bit FNV-1a prime
    }
    return hash;
}

// Create a new hash table
Stack* create_stack(int size) {
    Stack* ht = malloc(sizeof(Stack));
    ht->capacity = 16; // Initial capacity
    ht->resize = size;
    ht->size = 0;

    if (size == 0)
        ht->table = (Node **)calloc(ht->capacity, sizeof(Node *));
    else {
        if (size > 0)
            ht->table = (Node **)calloc(size, sizeof(Node *));
        else {
            ht->table = (Node **)calloc((-size), sizeof(Node *));
            ht->resize = 0;
            ht->capacity = -size; 
        }
    }

    return ht;
}

Stack* create_callStack(int size) {
    Stack* ht = malloc(sizeof(Stack));
    ht->table = (Node **)calloc(size, sizeof(Node *));
    ht->capacity = size;
    ht->resize = 0;
    ht->size = 0;

    return ht;
}

// Find the entry in the table
Node* find_entry(Stack* ht, const char* key) {
    if (ht->resize)
        return get_from_stack_i(ht, key);

    size_t index = hash(key) % ht->capacity;    
    Node* current = ht->table[index];
    
    // incase collisions occurred
    while (current != NULL && strcmp(current->key, key) != 0)
        current = current->next;
    
    return current;
}

void free_object(Bobject* data){
    switch (data->type) {
        case BNUMBER:
            free(data);
            break;
        default:
            break;
    }
}

void add_to_stack_i(Stack *ht, const char *key, Bobject *value) {
    size_t index = hash(key) % ht->resize;
    size_t originalIndex = index;

    // Linear probing to handle collisions
    while (ht->table[index] != NULL && strcmp(ht->table[index]->key, key) != 0) {
        index = (index + 1) % ht->resize;

        if (index == originalIndex) // Table is full
            return; // Insertion failed
    }

    Node* entry = malloc(sizeof(Node));
    entry->key = strdup(key);
    entry->value = value;

    ht->table[index] = entry;
}

void add_to_stack(Stack* ht, const char* key, Bobject* value) {
    if (ht->resize){
        add_to_stack_i(ht, key, value);
        return;
    }

    size_t index = hash(key) % ht->capacity;
    Node* current = ht->table[index];

    // Optimize for common case: no collision
    if (current == NULL) {
        Node* new_node = malloc(sizeof(Node));

        new_node->key = strdup(key);
        new_node->value = value;

        ++(value->refs);

        new_node->next = NULL;
        ht->table[index] = new_node;
        ht->size++;

        // Check if we need to resize (grow)
        if ((double)ht->size / ht->capacity >= 0.75) { // Load factor
            hash_table_resize(ht, ht->capacity * 2);
        }
        return;
    }

    // Handle collision
    while (current->next != NULL && strcmp(current->key, key) != 0) {
        current = current->next;
    }

    if (strcmp(current->key, key) == 0) { // Update existing entry
        if (current->value->type == BNUMBER){
            --(current->value->refs);

            if (current->value->refs == 0)
                free_object(current->value);
        }

        current->value = value;
        ++(value->refs);

    } else { // Add new entry
        Node* new_node = malloc(sizeof(Node));
        new_node->key = strdup(key);
        new_node->value = value;

        ++(value->refs);

        new_node->next = NULL;
        current->next = new_node;
        ht->size++;

        // Check if we need to resize (grow)
        if ((double)ht->size / ht->capacity >= 0.75) { // Load factor
            hash_table_resize(ht, ht->capacity * 2);
        }
    }
}

// retrieve from immutable stack
Node* get_from_stack_i(Stack *ht, const char *key) {
    size_t index = hash(key) % ht->resize;
    size_t originalIndex = index;

    while (ht->table[index] != NULL && index < ht->resize) {
        if (strcmp(ht->table[index]->key, key) == 0)
            return ht->table[index];

        index = (index + 1) % ht->resize;
     
        // not found
        if (index == originalIndex)
            return NULL;
    }

    return NULL;
}

// Get a value from the hash table
Bobject *get_from_stack(Stack* ht, const char* key) {
    Node* entry = find_entry(ht, key);
    return entry != NULL ? entry->value : NULL;
}

// Check if a key exists in the hash table
bool hash_table_exists(Stack* ht, const char* key) {
    return find_entry(ht, key) != NULL;
}

// Resize the hash table
void hash_table_resize(Stack* ht, size_t new_capacity) {
    Node** old_table = ht->table;
    size_t old_capacity = ht->capacity;

    ht->capacity = new_capacity;
    ht->table = (Node**)calloc(ht->capacity, sizeof(Node*));
    if (ht->table == NULL) {
        ht->table = old_table;
        return;
    }

    ht->size = 0; // Reset size during rehashing
    for (size_t i = 0; i < old_capacity; i++) {
        Node* current = old_table[i];
        while (current != NULL) {
            Node* next = current->next;
            add_to_stack(ht, current->key, current->value); // Rehash entries
            free(current->key);
            free(current);
            current = next;
        }
    }

    free(old_table);

    // Check if we need to shrink
    if ((double)ht->size / ht->capacity <= 0.25 && ht->capacity > 64) { // Shrink factor
        hash_table_resize(ht, ht->capacity / 2);
    }
}

// Copy the hash table (for new scope)
Stack* copy_stack(Stack* ht) {
    Stack* new_ht = create_stack(ht->capacity);

    for (size_t i = 0; i < ht->capacity; i++) {
        Node* current = ht->table[i];
        while (current != NULL) {
            add_to_stack(new_ht, current->key, current->value);
            current = current->next;
        }
    }

    return new_ht;
}

// Delete the hash table
void delete_stack(Stack* ht) {
    hash_table_free(ht);
}

void heap_free_object(Bobject *ob){
    --(ob->refs);

    if (ob->refs == 0){
        switch (ob->type){
            case BNUMBER:
                free(ob);
                break;

            case BSTRING:
                free(ob->value.str_value);
                free(ob);
                break;
            
            default:
                break;
        }
    }
}

// Free the hash table
void hash_table_free(Stack* ht) {
    if (ht->resize){
        for (size_t i = 0; i < ht->resize; i++) {
            Node* current = ht->table[i];

            heap_free_object(current->value);
            free(current->key);
            free(current);
        }
    } else {
        for (size_t i = 0; i < ht->capacity; i++) {
            Node* current = ht->table[i];
            while (current != NULL) {
                Node* next = current->next;

                heap_free_object(current->value);
                free(current->key);
                free(current);

                current = next;
            }
        }
    }
    free(ht->table);
    free(ht);
}

void print_stack(Stack* ht) {
    if (ht->resize){
        printf("[ ");
        for (size_t i = 0; i < ht->resize; i++) {
            Node* current = ht->table[i];

            if (current != NULL)
                printf("%s, ", current->key);
        }
        printf(" ]\n");
    } else {
        for (size_t i = 0; i < ht->capacity; i++) {
            Node* current = ht->table[i];

            if (current != NULL){
                printf("  [%zu]: ", i);
                while (current != NULL) {
                    printf("{%s: %p} -> ", current->key, current->value);
                    current = current->next;
                }
                printf("\n");
            }
        }
    }
}


