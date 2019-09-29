CFLAGS+=-g
all: lexer_tests

lexer_tests: lexer_tests.o token.o lexer.o
	$(CC) $(CFLAGS) -o lexer_tests lexer_tests.o token.o lexer.o

lexer_tests.o: lexer_tests.c
	$(CC) $(CFLAGS) -c lexer_tests.c

token.o: token.c
	$(CC) $(CFLAGS) -c token.c

lexer.o: lexer.c
	$(CC) $(CFLAGS) -c lexer.c

clean:
	rm -rf *.o lexer_tests
