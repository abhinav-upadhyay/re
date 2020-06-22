#ifndef TOKEN_H
#define TOKEN_H

typedef enum token_type {
    CHAR,
    DOT,
    PLUS,
    QUESTION,
    PIPE,
    CAT, //not a real token but represents a concat operator
    LPAREN,
    RPAREN,
    STAR,
    ILLEGAL,
    END_OF_FILE
} token_type;

static const char *token_names[] = {
    "CHAR",
    "DOT",
    "PLUS",
    "QUESTION",
    "PIPE",
    "CAT",
    "LPAREN",
    "RPAREN",
    "STAR",
    "ILLEGAL",
    "EOF"
};

typedef struct token_t {
    token_type type;
    char *literal;
} token_t;

#define get_token_name(tok_type) token_names[tok_type]
token_type get_token_type(const char *);
token_t *token_copy(token_t *);
void token_free(token_t *);

#endif