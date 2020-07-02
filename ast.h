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

#ifndef AST_H
#define AST_H

#include <stdint.h>
#include "token.h"

typedef enum node_type_t {
    EXPRESSION_NODE
} node_type_t;

static const char *node_type_strings[] = {
    "EXPRESSION"
};

#define node_to_string(node_type) node_type_strings[node_type]

typedef enum expression_type_t {
    CHAR_LITERAL,
    CHAR_CLASS,
    INFIX_EXPRESSION,
    POSTFIX_EXPRESSION,
    PREFIX_EXPRESSION
} expression_type_t;

static const char *expression_type_strings[] = {
    "CHAR",
    "CHAR_CLASS",
    "INFIX_EXPRESSION",
    "POSTFIX_EXPRESSION",
    "PREFIX_EXPRESSION"
};

#define expression_type_to_string(exp_type) expression_type_strings[exp_type]

typedef enum operator_t {
    OR,
    CONCAT,
    ONE_OR_MORE,
    ZERO_OR_ONE,
    ZERO_OR_MORE
} operator_t;

static const char *operator_strings[] = {
    "OR",
    "CONCAT",
    "+",
    "?",
    "*"
};

#define operator_to_string(op) operator_strings[op]


typedef struct node_t {
    node_type_t type;
    char * (*string) (struct node_t *);
    token_t *token;
} node_t;

typedef struct expression_node_t {
    node_t node;
    expression_type_t type;
} expression_node_t;

typedef struct char_literal_t {
    expression_node_t expression;
    char value;
} char_literal_t;

typedef struct char_class_t {
    expression_node_t expression;
    uint8_t allowed_values[256];
} char_class_t;

typedef struct infix_expression_t {
    expression_node_t expression;
    expression_node_t *left;
    expression_node_t *right;
    operator_t op;
} infix_expression_t;

typedef struct postfix_expression_t {
    expression_node_t expression;
    expression_node_t *left;
    operator_t op;
} postfix_expression_t;


typedef struct regex_t {
    node_t node;
    node_t *root;
} regex_t;

#endif
