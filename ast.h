#ifndef AST_H
#define AST_H

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
    INFIX_EXPRESSION,
    POSTFIX_EXPRESSION,
    PREFIX_EXPRESSION
} expression_type_t;

static const char *expression_type_strings[] = {
    "CHAR",
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
    "?"
    "*"
};

#define operator_to_string(op) operator_strings[op]


typedef struct node_t {
    node_type_t type;
    char *token_literal;
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
