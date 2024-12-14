
all:
	@gcc main.c \
	parser/lexer.c \
	parser/parser.c \
	parser/bcode.c \
	parser/bobject.c \
	parser/bytec.c \
    parser/eval.c \
    Modules/bapi.c \
    Modules/helpers.c \
    -lm \
    -o beacon

run:
	@./beacon test/main.bc

v:
	@valgrind ./beacon test/main.bc

g:
	@gdb ./beacon test/main.bc