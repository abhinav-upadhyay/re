CC=clang
CFLAGS+=-g -D_GNU_SOURCE
all: lexer_tests parser_tests

lexer_tests: lexer_tests.o token.o lexer.o
	$(CC) $(CFLAGS) -o lexer_tests lexer_tests.o token.o lexer.o

parser_tests: parser_tests.o token.o lexer.o parser.o
	$(CC) $(CFLAGS) -o parser_tests parser_tests.o token.o lexer.o parser.o

lexer_tests.o: lexer_tests.c
	$(CC) $(CFLAGS) -c lexer_tests.c

parser_tests.o: parser_tests.c
	$(CC) $(CFLAGS) -c parser_tests.c

token.o: token.c
	$(CC) $(CFLAGS) -c token.c

lexer.o: lexer.c
	$(CC) $(CFLAGS) -c lexer.c

parser.o: parser.c
	$(CC) $(CFLAGS) -c parser.c

clean:
	rm -rf *.o lexer_tests core
