#ifndef TOKEN_H
#define TOKEN_H

typedef enum token_type {
    CHAR,
    DOT,
    CARET,
    DOLLAR,
    PLUS,
    QUESTION,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    DIGIT_CLASS,
    NON_DIGIT_CLASS,
    WHITE_SPACE_CLASS,
    NON_WHITE_SPACE_CLASS,
    ALPHANUMERIC_CLASS,
    NON_ALPHANUMERIC_CLASS,
    PIPE,
    LPAREN,
    RPAREN,
    STAR,
    HYPHEN,
    ILLEGAL,
    END_OF_FILE
} token_type;

static const char *token_names[] = {
    "CHAR",
    "DOT",
    "CARET",
    "DOLLAR",
    "PLUS",
    "QUESTION",
    "LBRACE",
    "RBRACE",
    "LBRACKET",
    "RBRACKET",
    "DIGIT_CLASS",
    "NON_DIGIT_CLASS",
    "WHITE_SPACE_CLASS",
    "NON_WHITE_SPACE_CLASS",
    "ALPHANUMERIC_CLASS",
    "NON_ALPHANUMERIC_CLASS",
    "PIPE",
    "LPAREN",
    "RPAREN",
    "STAR",
    "HYPHEN",
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