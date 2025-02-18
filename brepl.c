#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

//
#include "Include/brepl.h"

//
#include "parser/Includes/bcode.h"
#include "parser/Includes/bstate.h"
#include "parser/Includes/bobject.h"
#include "parser/Includes/bytecode.h"
#include "parser/Includes/eval.h"
#include "parser/Includes/lexer.h"
#include "parser/Includes/parser.h"

#include "Modules/Includes/_list.h"
#include "Modules/Includes/_math.h"
#include "Modules/Includes/_modules.h"
#include "Modules/Includes/_share.h"
#include "Modules/Includes/_socket.h"
#include "Modules/Includes/_string.h"
#include "Modules/Includes/_time.h"
#include "Modules/Includes/_fs.h"

char* open_program_file(const char* filename) {
    FILE* code = fopen(filename, "rb");
    if (code == NULL) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        return NULL;
    }

    fseek(code, 0, SEEK_END);
    size_t file_size = ftell(code);
    rewind(code);

    char* buffer = malloc(file_size + 1);

    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        fclose(code);
        return NULL;
    }

    size_t bytes_read = fread(buffer, 1, file_size, code);
    
    if (bytes_read != file_size) {
        fprintf(stderr, "Error reading from file.\n");
        fclose(code);
        free(buffer);
        return NULL;
    }

    buffer[file_size] = '\0';

    fclose(code);
    return buffer;
}

void start_interpreter(const int argc, char *argv[]){
    switch (argc) {
        case 1:
            start_console();
            break;
        default:
            start_vm(argv);
            break;
    }
}

void initiate_modules(bcon_State *bstate){
    Stack* bheap = bstate->callStack[0];

    Bobject *share_module = Bn_Share();
    Stack *builtins = share_module->value.module->attrs;

    add_to_stack(builtins, "List", Bn_List());
    add_to_stack(builtins, "None", bstate->none);

    add_to_stack(bheap, "True", get_from_stack(builtins, "True"));
    add_to_stack(bheap, "False", get_from_stack(builtins, "False"));
    add_to_stack(bheap, "None", bstate->none);
    add_to_stack(bheap, "List", get_from_stack(builtins, "List"));
    add_to_stack(bheap, "print", get_from_stack(builtins, "print"));
    add_to_stack(bheap, "quit", get_from_stack(builtins, "quit"));
    add_to_stack(bheap, "input", get_from_stack(builtins, "input"));
    add_to_stack(bheap, "require", get_from_stack(builtins, "require"));
    add_to_stack(bheap, "type", get_from_stack(builtins, "type"));

    Stack *modules = create_stack(0);
    add_to_stack(modules, "Share", share_module);
    
    bstate->modules = modules;
}

bcon_State* make_bstate(char *file){
    bcon_State *bstate = malloc(sizeof(bcon_State));
    
    Bobject *None = malloc(sizeof(Bobject));
    None->refs = 1000000;
    None->type = BNONE;
    bstate->none = None;

    bstate->memory = create_stack(0);
    bstate->islocked = 2;
    bstate->file = file;

    bstate->callStack = malloc(sizeof(Stack*) * 1000);
    bstate->callStack[0] = create_stack(0);
    bstate->stackCapacity = 1000;
    bstate->stackPos = 0;

    // initiates after setting None
    initiate_modules(bstate);

    return bstate;
}

void run_executor(char* code, char* file_name, bcon_State* bstate){
    Lexer *lex = lexer_read(code, file_name);
    lexer_process(lex);

    Parser *par = parser_read(lex);
    parser_tree(par);
    free(lex);

    Inter *b_interpreter = inter_read(par);
    inter_interpret(b_interpreter);
    free(par);

    Eval *evaluator = evaluator_read(b_interpreter);
    evaluator_start(evaluator, bstate);

    free(b_interpreter);

    if(bstate->islocked == BLOCK_ERRORED){
        if (bstate->return_value == NULL){
            printf("Help send feedback for this missing error log.\n");

        } else {
            Stack *error = bstate->return_value->value.bface->attrs;

            printf("  File '%s'  line %d\n\n", get_from_stack(error, "file")->value.str_value,
                                               (int)(get_from_stack(error, "line")->value.num_value));

            switch ((int)(get_from_stack(error, "errno")->value.num_value)) {
                case BERRNO:
                    printf("%s: %s\n",  get_from_stack(error, "type")->value.str_value,
                                        get_from_stack(error, "message")->value.str_value );
                    break;
                default:
                    printf("%s: %s\n", get_from_stack(error, "type")->value.str_value,
                           strerror((int)(get_from_stack(error, "errno")->value.num_value)));
                    break;
            }
            free(bstate->return_value);
        }
    }
}

char* read_input(){
    char* line;
    printf(">> ");

    int ret = scanf("%s", line);
    
    if (ret == EOF)
        return NULL;

    return strdup(line);
}

void start_console(){
    bcon_State* bstate = make_bstate("__stdin__");

    printf("Hey its Beacon 0.0.1 Copyright (C) \n");
    char* code;

    while (1){
        code = read_input();

        if (code == NULL){
            printf("Encountered an input error, please retry.\n");
            exit(1);
        }

        run_executor(code, "__stdin__", bstate);
    }
}

char* mk_absolute_path(char* name){
    char *ret = realpath(name, NULL);

    if (ret == NULL)
        return name;

    errno = 0;
	return strdup(ret);
}

char* mk_base_folder(char* file) {
    size_t len = strlen(file);

    for (size_t i = len; i > 0; i--){
        if (file[i] == '/'){
            char *folder = malloc(i+2);
            memcpy(folder, file, i+1);
            folder[i+1] = '\0';

            return folder;
        }
    }

    char *folder2 = malloc(2);
    folder2[0] ='/';
    folder2[1] ='\0';

    return folder2;
}

void start_vm(char *argv[]){
    if (access(argv[1], F_OK)) {
        printf("beacon error: No such file '%s'", argv[1]);
        exit(1);
    }

    char* filename = mk_absolute_path(argv[1]);
    char *code = open_program_file(filename);

    bcon_State *bstate = make_bstate(filename);
    bstate->base_folder = mk_base_folder(filename);

    run_executor(code, filename, bstate);
}
