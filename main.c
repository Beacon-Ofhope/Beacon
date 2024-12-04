#include <stdio.h>
#include <stdlib.h>
#include "parser/lexer.h"
#include "parser/parser.h"
#include "parser/dcode.h"
#include "parser/dobject.h"
#include "parser/eval.h"
#include "parser/bytec.h"

#include "Include/_api.h"


char* file_code(char* file_name){
    FILE *fp;
    char *buffer;
    long fileLen;

    // Open the file in read mode
    fp = fopen(file_name, "r");

    if (fp == NULL) {
        perror("Error: Failed to open the file.");
        exit(0);
    }

    // Get the file size
    fseek(fp, 0, SEEK_END);
    fileLen = ftell(fp);
    rewind(fp);

    // Allocate memory for the buffer
    buffer = (char*)malloc(fileLen + 1); // +1 for null terminator
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


void main(int argc, char* argv[]){
	if (argc < 2){
		printf("Error: expects a file.");
	}

	char* code = file_code(argv[1]);
	
	Token* lexer_head = NULL;
	lexer_process(lexer_read(code), &lexer_head);

    AstNode* parser_head = NULL;
    parser_tree(parser_read(&lexer_head), &parser_head);

    Dcode* dcode_head = NULL;
    Interpo_start(Interpo_read(&parser_head), &dcode_head);

    Stack* store = create_stack();
    add_to_stack(store, "print", bt_make_b_function(&print));

    evaluator_start(evaluator_read(&dcode_head), store);

    // while (lexer_head != NULL) {
    //     printf("%d\n", lexer_head->type);
    //     lexer_head = lexer_head->next;
    // }
}

