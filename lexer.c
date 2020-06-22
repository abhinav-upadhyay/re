#include <err.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "token.h"

lexer_t *
lexer_init(const char *input)
{
    lexer_t *l;
    l = malloc(sizeof(*l));
    if (l == NULL)
        err(EXIT_FAILURE, "malloc failed");
    l->input = strdup(input);
    if (l->input == NULL)
        err(EXIT_FAILURE, "malloc failed");
    l->cur_offset = 0;
    l->read_offset = 1;
    l->ch = input[0];
    return l;
}

static void
read_char(lexer_t *l)
{
    if (l->ch) {
        l->cur_offset = l->read_offset;
        l->read_offset++;
        l->ch = l->input[l->cur_offset];
    }
}

token_t *
next_token(lexer_t *l)
{
    token_t *t;
    t = malloc(sizeof(*t));
    if (t == NULL)
        err(EXIT_FAILURE, "malloc failed");
    switch (l->ch) {
    case '.':
        t->type = DOT;
        t->literal = strdup(".");
        read_char(l);
        break;
    case '+':
        t->type = PLUS;
        t->literal = strdup("+");
        read_char(l);
        break;
    case '?':
        t->type = QUESTION;
        t->literal = strdup("?");
        read_char(l);
        break;
    case '(':
        t->type = LPAREN;
        t->literal = strdup("(");
        read_char(l);
        break;
    case ')':
        t->type = RPAREN;
        t->literal = strdup(")");
        read_char(l);
        break;
    case '*':
        t->type = STAR;
        t->literal = strdup("*");
        read_char(l);
        break;
    case '|':
        t->type = PIPE;
        t->literal = strdup("|");
        read_char(l);
        break;
    case 0:
        t->type = END_OF_FILE;
        t->literal = "";
        break;
    default:
        t->type = CHAR;
        t->literal = malloc(2);
        t->literal[0] = l->ch;
        t->literal[1] = 0;
        read_char(l);
        break;
    }

    if (t->type != ILLEGAL) {
        if (t->literal == NULL)
            err(EXIT_FAILURE, "malloc failed");
    }
    return t;
}

void
lexer_free(lexer_t *l)
{
    free(l->input);
    free(l);

}
