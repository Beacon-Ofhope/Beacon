
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
	parser/Bytecode/bdict.c \
	parser/Bytecode/blist.c \
	parser/Bytecode/bclass.c \
	parser/Bytecode/berrors.c \
	parser/Bytecode/bimport.c \
	parser/Bytecode/bfunction.c \
	parser/Bytecode/bstatements.c \
	\
    Modules/_modules.c \
	\
    Modules/_list.c \
    Modules/_share/_prints.c \
    Modules/_share/_share.c \
	\
    Modules/_import/_imports.c \
    Modules/_import/_imp_exec.c \
    Modules/_import/_imp_string_manuplator.c \
    -lm \
    -o beacon
	@mv beacon ../test/beacon

run:
	@./beacon test/main.bc

v:
	@valgrind ./beacon test/main.bc

g:
	@gdb ./beacon test/main.bc

# \
#     Modules/_list.c \
#     Modules/_math.c \
#     Modules/_string.c \
#     Modules/_random.c \
# 	\
# \
#     Modules/_socket/_socket.c \
#     Modules/_socket/_socket_connect.c \
# 	\
#     Modules/_time/_time.c \
#     Modules/_time/_time_data.c \
# 	\
#     Modules/_fs/_fs.c \
#     Modules/_fs/_file.c \