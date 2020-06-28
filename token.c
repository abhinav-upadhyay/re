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