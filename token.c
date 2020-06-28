#include <err.h>
#include <string.h>
#include <stdlib.h>
#include "token.h"

token_type
get_token_type(const char *tok_literal)
{
    if (tok_literal[0] == 0)
        return END_OF_FILE;

    switch (tok_literal[0]) {
    case '.':
        return DOT;
    case '+':
        return PLUS;
    case '?':
        return QUESTION;
    case '(':
        return LPAREN;
    case ')':
        return RPAREN;
    case '|':
        return PIPE;
    case '*':
        return STAR;
    default:
        return CHAR;
    }

    return CHAR;
}

token_t *
token_copy(token_t *tok)
{
    token_t *new_token;
    new_token = malloc(sizeof(*new_token));
    if (new_token == NULL)
        err(EXIT_FAILURE, "malloc failed");
    new_token->type = tok->type;
    char *literal = strdup(tok->literal);
    if (literal == NULL) 
        err(EXIT_FAILURE, "malloc failed");
    new_token->literal = literal;
    return new_token;
}

void
token_free(token_t *tok)
{
    if (tok->type != END_OF_FILE)
        free(tok->literal);
    free(tok);
}