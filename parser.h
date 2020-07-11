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

#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"
#include "token.h"
#include "re_utils.h"

typedef struct parser_t {
    lexer_t *lexer;
    token_t *cur_tok;
    token_t *peek_tok;
    char *error;
} parser_t;


typedef enum operator_precedence_t {
    LOWEST=1,
    PRE_OR,
    PRE_CAT,
    PRE_ASTERISK // basically all repitition meta characters
} operator_precedence_t;

typedef expression_node_t * (*infix_parse_fn) (parser_t *, expression_node_t *);
typedef expression_node_t * (*postfix_parse_fn) (parser_t *, expression_node_t *);
typedef expression_node_t * (*prefix_parse_fn) (parser_t *);

parser_t *parser_init(lexer_t *);
void parser_next_token(parser_t *);
regex_t *parse_regex(parser_t *);
regex_t *regex_init(void);
void print_ast(regex_t *);
void parser_free(parser_t *);
void regex_free(regex_t *);
postfix_expression_t *create_postfix_exp(void);
infix_expression_t *create_infix_exp(void);
char_literal_t *create_char_literal(void);
char_class_t *create_char_class(void);
void free_expression(expression_node_t *);
expression_node_t *copy_expression(expression_node_t *);

#endif