/*-
 * Copyright (c) 2020 Abhinav Upadhyay <er.abhinav.upadhyay@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

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
    case '+':
        t->type = PLUS;
        t->literal = "+";
        read_char(l);
        break;
    case '?':
        t->type = QUESTION;
        t->literal = "?";
        read_char(l);
        break;
    case '(':
        t->type = LPAREN;
        t->literal = "(";
        read_char(l);
        break;
    case ')':
        t->type = RPAREN;
        t->literal = ")";
        read_char(l);
        break;
    case '*':
        t->type = STAR;
        t->literal = "*";
        read_char(l);
        break;
    case '|':
        t->type = PIPE;
        t->literal = "|";
        read_char(l);
        break;
    case '[':
        t->type = LBRACKET;
        t->literal = "[";
        read_char(l);
        break;
    case ']':
        t->type = RBRACKET;
        t->literal = "]";
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
