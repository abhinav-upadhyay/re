#include <err.h>
#include <string.h>
#include <stdlib.h>
#include "token.h"

token_type
get_token_type(const char *tok_literal)
{
    if (tok_literal[0] == 0)
        return END_OF_FILE;

    if (tok_literal[1] == 0) {
        switch (tok_literal[0]) {
        case '.':
            return DOT;
        case '^':
            return CARET;
        case '$':
            return DOLLAR;
        case '+':
            return PLUS;
        case '?':
            return QUESTION;
        case '{':
            return LBRACE;
        case '}':
            return RBRACE;
        case '[':
            return LBRACKET;
        case ']':
            return RBRACKET;
        case '(':
            return LPAREN;
        case ')':
            return RPAREN;
        case '|':
            return PIPE;
        case '*':
            return STAR;
        case '-':
            return HYPHEN;
        default:
            return CHAR;
        }
    }

    if (tok_literal[2] == 0 && tok_literal[0] == '\\') {
        switch (tok_literal[1]) {
            case 'd':
                return DIGIT_CLASS;
            case 'D':
                return NON_DIGIT_CLASS;
            case 's':
                return WHITE_SPACE_CLASS;
            case 'S':
                return NON_WHITE_SPACE_CLASS;
            case 'w':
                return ALPHANUMERIC_CLASS;
            case 'W':
                return NON_ALPHANUMERIC_CLASS;
            default:
                return CHAR;
        }
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
    free(tok->literal);
    free(tok);
}