#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include "token.h"

typedef struct lexer_t {
    char *input;
    size_t cur_offset;
    size_t read_offset;
    char ch;
} lexer_t;

lexer_t *lexer_init(const char *);
token_t *next_token(lexer_t *);
token_t *peek_token(lexer_t *);
void lexer_free(lexer_t *);

#endif