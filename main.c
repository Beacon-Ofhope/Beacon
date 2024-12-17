#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include "Include/bapi.h"
#include "parser/bcode.h"
#include "parser/bobject.h"
#include "parser/bytec.h"
#include "parser/eval.h"
#include "parser/lexer.h"
#include "parser/parser.h"


char* file_code(const char* file_name){
	// Open the file in read mode
    FILE *fp = fopen(file_name, "r");

    if (fp == NULL) {
        perror("Error: Failed to open the file.");
        exit(0);
    }

    // Get the file size
    fseek(fp, 0, SEEK_END);
    const long fileLen = ftell(fp);
    rewind(fp);

    // Allocate memory for the buffer
    char *buffer = (char *) malloc((fileLen + 1)); // +1 for null terminator
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


void main(const int argc, char* argv[]){
    if (argc < 2){
	    fprintf(stderr, "Error: no file path is given.");
	    exit(0);
    }
    char* code = file_code(argv[1]);

    Lexer* lex = lexer_read(code, argv[1]);
    lexer_process(lex);

    Parser* par = parser_read(lex);
    parser_tree(par);

    Inter * b_interpreter = inter_read(par);
	inter_interpret(b_interpreter);

    Stack * memory = create_stack();
    add_to_stack(memory, "print", bt_make_b_fun(print));
    add_to_stack(memory, "_file", bt_mk_string(argv[1]));

    Eval * evaluator = evaluator_read(b_interpreter, memory);
    evaluator_start(evaluator);

}

