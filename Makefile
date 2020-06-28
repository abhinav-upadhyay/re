CC=clang
CFLAGS+=-g -D_GNU_SOURCE
all: lexer_tests parser_tests nfa_executor_tests benchmark

lexer_tests: lexer_tests.o token.o lexer.o
	$(CC) $(CFLAGS) -o lexer_tests lexer_tests.o token.o lexer.o

parser_tests: parser_tests.o token.o lexer.o parser.o
	$(CC) $(CFLAGS) -o parser_tests parser_tests.o token.o lexer.o parser.o

nfa_executor_tests: nfa_executor_tests.o nfa_executor.o nfa_compiler.o parser.o lexer.o token.o re_utils.o
	$(CC) $(CFLAGS) -o nfa_executor_tests nfa_executor_tests.o nfa_executor.o nfa_compiler.o parser.o lexer.o token.o re_utils.o

benchmark: benchmark.o nfa_executor.o nfa_compiler.o parser.o lexer.o token.o re_utils.o
	$(CC) $(CFLAGS) -o benchmark benchmark.o nfa_executor.o nfa_compiler.o parser.o lexer.o token.o re_utils.o


lexer_tests.o: lexer_tests.c
	$(CC) $(CFLAGS) -c lexer_tests.c

parser_tests.o: parser_tests.c
	$(CC) $(CFLAGS) -c parser_tests.c

nfa_executor_tests.o: nfa_executor_tests.c
	$(CC) $(CFLAGS) -c nfa_executor_tests.c

token.o: token.c
	$(CC) $(CFLAGS) -c token.c

lexer.o: lexer.c
	$(CC) $(CFLAGS) -c lexer.c

parser.o: parser.c
	$(CC) $(CFLAGS) -c parser.c

nfa_compiler.o: nfa_compiler.c
	$(CC) $(CFLAGS) -c nfa_compiler.c

nfa_executor.o: nfa_executor.c
	$(CC) $(CFLAGS) -c nfa_executor.c

benchmark.o: benchmark.c
	$(CC) $(CFLAGS) -c benchmark.c

re_utils.o: re_utils.c
	$(CC) $(CFLAGS) -c re_utils.c

clean:
	rm -rf *.o lexer_tests core
