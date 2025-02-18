#ifndef H_BREPL
#define H_BREPL

char *open_program_file(const char *file_name);

char* mk_base_folder(char* file);

void start_interpreter(const int argc, char *argv[]);

void start_vm(char *argv[]);

void start_console();

#endif