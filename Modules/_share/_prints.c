#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Includes/_share.h"
#include "../Includes/_imports.h"
#include "../Includes/_modules.h"
#include "../../parser/Includes/bobject.h"
#include "../../parser/Includes/bytecode.h"

void print_string_share(Bobject* arg){
    printf("%s", arg->value.str_value);
}

void print_type_share(Bobject* arg){
    printf("%s", arg->value.btype->name);
}

void print_number_share(Bobject* arg){
    printf("%g", arg->value.num_value);
}

void print_face_share(Bobject* arg){
    printf("<interface %d>", arg->value.bface->count);
}

void print_data_share(Bobject* arg){
    switch (arg->type){
        case BNUMBER:
            print_number_share(arg);
            break;
        case BSTRING:
            print_string_share(arg);
            break;
        case BLIST:
            print_list_share(arg);
            break;
        case BNONE:
            printf("None");
            break;
        case BTYPE:
            print_type_share(arg);
            break;
        case BDICT:
            print_dictionary_share(arg);
            break;
        case BINTERFACE:
            print_face_share(arg);
            break;
        case BMODULE:
            printf("<module '%s' at %p>", arg->value.module->name, arg);
            break;
        case BFUNCTION2:
            printf("<function '%s' at %p>", arg->value.bfun->name, arg);
            break;
        case BFUNCTION:
            printf("<function '%s' at %p>", arg->value.bfun2->name, arg);
            break;
        case BOBJECT:
            printf("<object '%s' at %p>", arg->value.bclass->name, arg);
            break;
        default:
            printf("%s", arg->value.str_value);
            break;
    }
}

void print_list_share(Bobject* arg){
    blist* l = arg->value.list;
    Bobject** members = l->values;

    size_t count = l->count;
    size_t count_stop = count-1;
    putchar('[');

    for(size_t i= 0; i < (count); ++i){
        if (members[i]->type == BSTRING){
            putchar('\'');
            print_data_share(members[i]);
            putchar('\'');
        } else {
            print_data_share(members[i]);
        }

        if (i == count_stop)
            break;

        printf(", ");
    }
    putchar(']');
}

void print_dictionary_share(Bobject* arg){
    Stack *ht = arg->value.attrs;

    Node **dt = ht->table;
    size_t capacity = ht->capacity;

    Node *current;
    int start = 0;
    putchar('{');

    for (size_t i = 0; i < capacity; ++i) {
        current = dt[i];

        if (current != NULL){
            while (current != NULL) {
                if (start)
                    printf(", ");
                else {
                    putchar(' ');
                    start = 1;
                }

                printf("%s: ", current->key);
                print_data_share(current->value);
                current = current->next;
            }
        }
    }

    putchar(' ');
    putchar('}');
}