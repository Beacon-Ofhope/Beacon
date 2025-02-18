#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Includes/bcode.h"
#include "../Includes/bobject.h"
#include "../Includes/bytecode.h"


void resize_list(blist* list){
    size_t size = list->capacity;
    list->capacity = (size == 0) ? 6: (size * 2);
    list->values = (Bobject**)realloc(list->values, list->capacity * sizeof(Bobject**));

    if (list->values == NULL) {
        perror("realloc failed at list resizing.");
        exit(EXIT_FAILURE);
    }
}

void shrink_list(blist* list){
    list->capacity /= 2;
    list->values = realloc(list->values, list->capacity * sizeof(Bobject**));

    if (!list->values) {
        perror("realloc failed at List shrinking.");
        exit(EXIT_FAILURE);
    }
}

void rehash_list(blist* list){
    Bobject** set = list->values;
    size_t capacity = list->capacity;
    size_t count = 0;

    Bobject* member;

    for(size_t i = 0; i < capacity; ++i){
        member = set[i];

        if (member){
            set[count] = member;
            ++count;
        }
    }
}

int append_to_list(blist* List, Bobject* value){
    size_t count = List->count;
    size_t count2 = List->capacity-1;

    if (count == List->capacity)
        resize_list(List);

    List->values[count] = value;
    ++(value->refs);
    ++(List->count);
    return 0;
}

Bobject* pop_list(blist* List, Bobject* index, bcon_State* bstate){
    if (List->capacity == 0)
        return _runtime_error(bstate, "cannot pop element from beyond List range.");

    size_t i = List->count - 1;
    char ended = 0;

    if (index != NULL){
        double j = index->value.num_value;

        if (j < 0)
            j = List->count + j;

        i = (size_t)(j);
        ended = 1;
    }

    if (i >= List->count || i < 0)
        return _runtime_error(bstate, "can't pop element from List beyond its range.");

    Bobject* popped = List->values[i];
    List->values[i] = NULL;

    if (i != (List->count)-1)
        rehash_list(List);

    --(List->count);
    --(popped->refs);

    if (List->count > 0 && List->count <= (List->capacity / 3))
        shrink_list(List);

    return popped;
}

Bobject* insert_in_list(blist* List, Bobject* value, size_t index, bcon_State* bstate) {
    if (index > List->count)
        return _runtime_error(bstate, "insert index is out of List bounds.");

    if (List->count == List->capacity)
        resize_list(List);

    size_t counter = List->count + 1;
    Bobject** set = List->values;

    Bobject* recent = set[index];
    set[index] = value;

    Bobject* be4;

    for(size_t i = (index+1); i < counter; ++i){
        be4 = set[i];
        set[i] = recent;
        recent = be4;
    }

    ++(List->count);
    return bstate->none;
}

Bobject* copy_list(blist* List, bcon_State* bstate){
    return bstate->none;
}


Bobject *bt_list(Bcode *code, bcon_State *bstate){
    size_t size = code->line + 5;
    Bobject** list_data = calloc(size, sizeof(Bobject*));

    Bcode *data = code->value.data;
    size_t count = 0;

    while (data != NULL) {
        list_data[count] = data->func(data, bstate);
        data = data->next;
        count++;
    }

    blist* list = malloc(sizeof(blist));
    list->count = count;
    list->capacity = size;
    list->values = list_data;

    Bobject *bin = mk_Bobject(bstate);
    bin->value.list = list;
    bin->type = BLIST;

    return bin;
}
