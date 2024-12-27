#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

//
#include "Include/bapi.h"
#include "Include/brepl.h"

//
#include "parser/Includes/bcode.h"
#include "parser/Includes/bstate.h"
#include "parser/Includes/bobject.h"
#include "parser/Includes/bytecode.h"
#include "parser/Includes/eval.h"
#include "parser/Includes/lexer.h"
#include "parser/Includes/parser.h"

#include "Modules/Includes/_string.h"

char* open_program_file(const char* file_name){
	// Open the file in read mode
    FILE *fp = fopen(file_name, "r");

    if (fp == NULL) {
        printf("beacon: [Errno %d]: %s ... '%s'\n", errno, strerror(errno), file_name);
        exit(0);
    }

    // Get the file size
    fseek(fp, 0, SEEK_END);
    const long fileLen = ftell(fp);
    rewind(fp);

    // Allocate memory for the buffer
    char *buffer = malloc((fileLen + 1)); // +1 for null terminator
    if (buffer == NULL) {
        perror("Error: Failed to allocate memory");
        fclose(fp);
        exit(0);
    }

    // Read the entire file into the buffer
    fread(buffer, 1, fileLen, fp);
    buffer[fileLen] = '\0';

    // Close the file
    fclose(fp);
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

bcon_State* make_bstate(){
    bcon_State *bstate = calloc(sizeof(bcon_State), 1);

    Stack *memory = create_stack();
    add_to_stack(memory, "print", bt_make_b_fun(print));
    add_to_stack(memory, "input", bt_make_b_fun(input));
    add_to_stack(memory, "string", M_String());

    bstate->memory = memory;
    bstate->islocked = 2;

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
}

char* read_input(){
    char* line = malloc(1025);
    printf(">> ");
    fgets(line, 1024, stdin);
   
    return line;
}

void start_console(){
    int run = 1;
    bcon_State* bstate = make_bstate();

    printf("Hey its Beacon 0.0.1 Copyright (C) \n");

    while (run){
        run_executor(read_input(), "__stdin__", bstate);
    }
}

void start_vm(char *argv[]){
    char *code = open_program_file(argv[1]);
    bcon_State *bstate = make_bstate();

    run_executor(code, argv[1], bstate);
}
