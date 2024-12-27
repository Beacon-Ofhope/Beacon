
all:
	@gcc main.c \
	brepl.c \
	\
	parser/lexer.c \
	parser/parser.c \
	parser/bcode.c \
	parser/bobject.c \
    parser/eval.c \
	\
	parser/Bytecode/bgen.c \
	parser/Bytecode/bdata.c \
	parser/Bytecode/bclass.c \
	parser/Bytecode/bfunction.c \
	parser/Bytecode/bstatements.c \
	\
    Modules/bapi.c \
    Modules/_string.c \
    -lm \
    -o beacon

run:
	@./beacon test/main.bc

v:
	@valgrind ./beacon test/main.bc

g:
	@gdb ./beacon test/main.bc