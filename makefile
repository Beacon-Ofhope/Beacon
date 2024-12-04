
all:
	@gcc main.c \
	parser/lexer.c \
	parser/parser.c \
	parser/dcode.c \
	parser/bytec.c \
	parser/dobject.c \
	parser/eval.c \
	Modules/_api.c \
	-lm \
	-o main && ./main test/test.txt

r:
	@./main test/test.txt