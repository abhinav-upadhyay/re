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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"

static char *expression_to_string(expression_node_t *);
static char * to_string(expression_node_t *);
static expression_node_t * parse_infix_expression(parser_t *, expression_node_t *);
static expression_node_t * parse_postfix_expression(parser_t *, expression_node_t *);
static expression_node_t * parse_char_node(parser_t *);
static expression_node_t * parse_re_group(parser_t *);
static expression_node_t * parse_char_class(parser_t *);
static void print_exp(expression_node_t *, size_t);

static prefix_parse_fn prefix_fns[] = {
    parse_char_node, // char
    NULL, // plus
    NULL, // question
    NULL, // pipe
    NULL, // cat
    parse_re_group, // lparen
    NULL, // rparen
    NULL, // star
    parse_char_class, // lbracket
    NULL, // rbracket
    NULL, // illegal
    NULL, // EOF
};

static postfix_parse_fn postfix_fns[] = {
    NULL, // char
    parse_postfix_expression, // plus
    parse_postfix_expression, // question
    NULL, // pipe
    NULL, // cat
    NULL, // lparen
    NULL, // rparen
    parse_postfix_expression, // star
    NULL, // lbracket
    NULL, // rbracket
    NULL, // illegal
    NULL // EOF
};

static infix_parse_fn infix_fns[] = {
    parse_infix_expression, // char
    NULL, // plus
    NULL, // question
    parse_infix_expression, // pipe
    parse_infix_expression, // cat
    parse_infix_expression, // lparen
    NULL, // rparen
    NULL, // star
    parse_infix_expression, // lbracket
    NULL, // rbracket
    NULL, // illegal
    NULL // EOF
};

static operator_precedence_t precedences[] = {
    LOWEST, // char
    PRE_ASTERISK, // plus
    PRE_ASTERISK, // question
    PRE_OR, // pipe
    PRE_CAT, // cat
    LOWEST, // lparen
    LOWEST, // rparen
    PRE_ASTERISK, // star
    LOWEST, // lbracket
    LOWEST, // rbracket
    LOWEST, // illegal
    LOWEST // EOF
};

#define get_precedence(toktype) precedences[toktype]
#define peek_precedence(parser) (parser->peek_tok->type == CHAR || parser->peek_tok->type == LPAREN \
    || parser->peek_tok->type == LBRACKET? precedences[CAT]: precedences[parser->peek_tok->type])


parser_t *
parser_init(lexer_t *lexer)
{
    parser_t *parser;
    parser = malloc(sizeof(*parser));
    if (parser == NULL)
        err(EXIT_FAILURE, "malloc failed");
    parser->lexer = lexer;
    parser->cur_tok = NULL;
    parser->peek_tok = NULL;
    parser->error = NULL;
    parser_next_token(parser);
    parser_next_token(parser);
    return parser;
}

regex_t *
regex_init(void)
{
    regex_t *regex;
    regex = malloc(sizeof(*regex));
    if (regex == NULL)
        err(EXIT_FAILURE, "malloc failed");
    regex->root = NULL;
    return regex;
}

void
parser_next_token(parser_t *parser)
{
    if (parser->cur_tok)
        token_free(parser->cur_tok);
    parser->cur_tok = parser->peek_tok;
    parser->peek_tok = next_token(parser->lexer);
}

static operator_t
get_op(token_type toktype)
{
    switch (toktype) {
    case CHAR:
        return CONCAT;
    case PIPE:
        return OR;
    case PLUS:
        return ONE_OR_MORE;
    case STAR:
        return ZERO_OR_MORE;    
    case QUESTION:
        return ZERO_OR_ONE;
    default:
        errx(EXIT_FAILURE, "Looking for operator for wrong token type");
    }
}

static expression_node_t *
parse_expression(parser_t *parser, operator_precedence_t precedence, token_type terminator_tok)
{
    expression_node_t *left;
    prefix_parse_fn prefix_fn = prefix_fns[parser->cur_tok->type];

    /* Expect the first token to be a char literal */
    if (prefix_fn == NULL) {
        char *error = NULL;
        asprintf(&error, "Unexpected character found %s", parser->cur_tok->literal);
        parser->error = error;
        return NULL;
    }
    left = prefix_fn(parser);
    if (parser->error)
        return NULL;
    
    while (parser->peek_tok->type != terminator_tok) {
        if (precedence >= peek_precedence(parser))
            break;
        infix_parse_fn infix_fn = infix_fns[parser->peek_tok->type];
        if (infix_fn) {
            parser_next_token(parser);
            expression_node_t *right = infix_fn(parser, left);
            if (parser->error)
                return NULL;
            left = right;
            continue;
        }
        postfix_parse_fn postfix_fn = postfix_fns[parser->peek_tok->type];
        if (postfix_fn) {
            parser_next_token(parser);
            expression_node_t *right = postfix_fn(parser, left);
            if (parser->error)
                return NULL;
            left = right;
            continue;
        }
        break;
    }
    return left;
}

postfix_expression_t *
create_postfix_exp(void)
{
    postfix_expression_t *postfix_exp;
    postfix_exp = malloc(sizeof(*postfix_exp));
    if (postfix_exp == NULL)
        err(EXIT_FAILURE, "malloc failed");
    postfix_exp->expression.type = POSTFIX_EXPRESSION;
    postfix_exp->expression.string = to_string;
    return postfix_exp;
}

static expression_node_t *
parse_postfix_expression(parser_t *parser, expression_node_t *left)
{
    operator_t op = get_op(parser->cur_tok->type);
    if (op == ZERO_OR_ONE) {
        infix_expression_t *infix_exp = create_infix_exp();
        infix_exp->op = OR;
        char_literal_t *null_node = create_char_literal();
        null_node->value = NULL_STATE;
        infix_exp->left = (expression_node_t *) null_node;
        infix_exp->right = left;
        return (expression_node_t *) infix_exp;
    }
    if (op == ONE_OR_MORE) {
        infix_expression_t *infix_exp = create_infix_exp();
        infix_exp->op = CONCAT;
        infix_exp->left = left;
        postfix_expression_t *right = create_postfix_exp();
        right->op = ZERO_OR_MORE;
        right->left = copy_expression(left);
        infix_exp->right = (expression_node_t *) right;
        return (expression_node_t *) infix_exp;
    }
    postfix_expression_t *postfix_exp = create_postfix_exp();
    postfix_exp->left = left;
    postfix_exp->op = get_op(parser->cur_tok->type);
    return (expression_node_t *) postfix_exp;
}

infix_expression_t *
create_infix_exp(void)
{
    infix_expression_t *infix_exp;
    infix_exp = malloc(sizeof(*infix_exp));
    if (infix_exp == NULL)
        err(EXIT_FAILURE, "malloc failed");
    infix_exp->expression.type = INFIX_EXPRESSION;
    infix_exp->expression.string = to_string;
    return infix_exp;
}

static expression_node_t *
parse_infix_expression(parser_t *parser, expression_node_t *left)
{
    operator_precedence_t precedence;
    infix_expression_t *infix_exp = create_infix_exp();
    infix_exp->left = left;
    token_type terminating_tok = parser->cur_tok->type == LPAREN? RPAREN: END_OF_FILE;
    if (parser->cur_tok->type == CHAR_LITERAL || parser->cur_tok->type == LPAREN || parser->cur_tok->type == LBRACKET) {
        infix_exp->op = CONCAT;
        precedence = get_precedence(CAT);
    } else {
        infix_exp->op = get_op(parser->cur_tok->type);
        precedence = get_precedence(parser->cur_tok->type);
        parser_next_token(parser);
    }
    infix_exp->right = parse_expression(parser, precedence, terminating_tok);
    return (expression_node_t *) infix_exp;
}

static expression_node_t *
parse_char_class(parser_t *parser)
{
    char_class_t *char_class_node = create_char_class();
    parser_next_token(parser);
    char prev_char_value = 0;
    if (parser->cur_tok->type == RBRACKET) {
        char_class_node->allowed_values[']'] = 1;
        prev_char_value = ']';
        parser_next_token(parser);
    }

    while (parser->cur_tok->type != RBRACKET && parser->cur_tok->type != END_OF_FILE) {
        if (parser->cur_tok->type != CHAR_LITERAL) {
            char *error = NULL;
            asprintf(&error, "Unexpected token type %s inside a character class", get_token_name(parser->cur_tok->type));
            parser->error = error;
            return NULL;
        }
        uint8_t value = (uint8_t) parser->cur_tok->literal[0];
        if (value == '-' && prev_char_value) {
            if (parser->peek_tok->type == CHAR_LITERAL) {
                parser_next_token(parser);
                uint8_t range_end = parser->cur_tok->literal[0];
                if (value >= range_end) {
                    char *error = NULL;
                    asprintf(&error, "Bad range");
                    parser->error = error;
                    return NULL;
                }
                for (uint8_t c = prev_char_value + 1; c <= range_end; c++)
                    char_class_node->allowed_values[c] = 1;
                prev_char_value = 0; //reset, so that we can parse more ranges
            } else if (parser->peek_tok->type == RBRACKET) {
                char_class_node->allowed_values['-'] = 1;
            }
        } else {
            char_class_node->allowed_values[value] = 1;
            prev_char_value = value;
        }
        parser_next_token(parser);
    }
    if (parser->cur_tok->type != RBRACKET) {
        char *error = NULL;
        asprintf(&error, "Missing matching ]");
        parser->error = error;
        return NULL;
    }
    return (expression_node_t *) char_class_node;
}

static expression_node_t *
parse_re_group(parser_t *parser)
{
    parser_next_token(parser);
    expression_node_t *exp = parse_expression(parser, LOWEST, RPAREN);
    if (exp == NULL) {
        char *error = NULL;
        asprintf(&error, "Invalid expression, missing matching `('");
        parser->error = error;
        return NULL;
    }
    if (parser->peek_tok->type != RPAREN)
        errx(EXIT_FAILURE, "Missing a matching )");
    parser_next_token(parser);
    return exp;
}

char_class_t *
create_char_class(void)
{
    char_class_t *char_class;
    char_class = calloc(1, sizeof(*char_class));
    if (char_class == NULL)
        err(EXIT_FAILURE, "malloc failed");
    char_class->expression.type = CHAR_CLASS;
    char_class->expression.string = to_string;
    return char_class;
}

char_literal_t *
create_char_literal(void)
{
    char_literal_t *char_node;
    char_node = malloc(sizeof(*char_node));
    if (char_node == NULL)
        err(EXIT_FAILURE, "malloc failed");
    char_node->expression.type = CHAR_LITERAL;
    char_node->expression.string = to_string;
    return char_node;
}

static expression_node_t *
parse_char_node(parser_t *parser)
{
    char_literal_t *char_node = create_char_literal();
    char_node->value = parser->cur_tok->literal[0];
    return (expression_node_t *) char_node;
}


regex_t *
parse_regex(parser_t *parser)
{
    regex_t * regex = regex_init();
    expression_node_t *node = parse_expression(parser, LOWEST, END_OF_FILE);
    regex->root = node;
    return regex;
}

void
parser_free(parser_t *parser)
{
    token_free(parser->cur_tok);
    token_free(parser->peek_tok);
    lexer_free(parser->lexer);
    free(parser);
}



void
regex_free(regex_t *regex)
{
    free_expression((expression_node_t *) regex->root);
    free(regex);
}

void
free_expression(expression_node_t *exp)
{
    infix_expression_t *infix;
    postfix_expression_t *postfix;
    switch (exp->type) {
    case INFIX_EXPRESSION:
        infix = (infix_expression_t *) exp;
        free_expression(infix->left);
        free_expression(infix->right);
        free(exp);
        return;
    case POSTFIX_EXPRESSION:
        postfix = (postfix_expression_t *) exp;
        free_expression(postfix->left);
        free(exp);
        return;
    default:
        free(exp);
    }
}

static char *
char_to_string(char_literal_t *node)
{
    char *s = malloc(sizeof(char) * 2);
    if (s == NULL)
        err(EXIT_FAILURE, "malloc failed");
    s[0] = node->value;
    s[1] = 0;
    return s;
}

static char *
char_class_to_string(char_class_t *node)
{
    size_t len = 0;
    char *s;
    for (size_t i = 0; i < 256; i++) {
        if (node->allowed_values[i])
            len++;
    }
    s = malloc(len + 1);
    if (s == NULL)
        err(EXIT_FAILURE, "malloc failed");
    for (size_t i = 0, j = 0; i < 256; i++) {
        if (node->allowed_values[i])
            s[j++] = i;
    }
    s[len] = 0;
    return s;
}

static char *
infix_to_string(infix_expression_t *node)
{
    char *left = expression_to_string(node->left);
    char *right = expression_to_string(node->right);
    char *s = NULL;
    asprintf(&s, "%s %s %s", left, operator_to_string(node->op), right);
    return s;
}

static char *
postfix_to_string(postfix_expression_t *node)
{
    char *left = expression_to_string(node->left);
    char *s = NULL;
    asprintf(&s, "%s %s", left, operator_to_string(node->op));
    return s;
}

static char *
expression_to_string(expression_node_t *node)
{
    switch (node->type) {
    case INFIX_EXPRESSION:
        return infix_to_string((infix_expression_t *) node);
    case POSTFIX_EXPRESSION:
        return postfix_to_string((postfix_expression_t *) node);
    case CHAR_LITERAL:
        return char_to_string((char_literal_t *) node);
    case CHAR_CLASS:
        return char_class_to_string((char_class_t *) node);
    default:
        return NULL;
    }
}

static char *
to_string(expression_node_t *node)
{
    return expression_to_string((expression_node_t *) node);
}

static void
do_indent(size_t indent)
{
    for (size_t i = 0; i < indent; i++)
        printf(" ");
}

static void
print_infix(infix_expression_t *exp, size_t indent)
{
    print_exp(exp->left, indent + 2);
    do_indent(indent);
    switch (exp->op) {
    case CONCAT:
        printf("CAT\n");
        break;
    case OR:
        printf("|\n");
        break;
    default:
        break;    
    }
    print_exp(exp->right, indent + 2);
}

static void
print_postfix(postfix_expression_t *exp, size_t indent)
{
    print_exp(exp->left, indent + 2);
    do_indent(indent);
    switch (exp->op) {
    case ZERO_OR_MORE:
        printf("*\n");
        break;
    case ZERO_OR_ONE:
        printf("?\n");
        break;
    case ONE_OR_MORE:
        printf("+\n");
        break;
    default:
        break;    
    }
}

static void
print_char_literal(char_literal_t *exp, size_t indent)
{
    do_indent(indent);
    printf("%c\n", exp->value);
}

static void
print_exp(expression_node_t *exp, size_t indent)
{
    switch (exp->type) {
    case INFIX_EXPRESSION:
        print_infix((infix_expression_t *) exp, indent + 2);
        break;
    case POSTFIX_EXPRESSION:
        print_postfix((postfix_expression_t *) exp, indent + 2);
        break;
    case CHAR_LITERAL:
        print_char_literal((char_literal_t *) exp, indent + 2);
        break;
    default:
        break;    
    }

}

void
print_ast(regex_t *regex)
{
    expression_node_t *exp = (expression_node_t *) regex->root;
    size_t indent = 0;
    print_exp(exp, indent);
}

expression_node_t *
copy_char_literal(char_literal_t *exp)
{
    char_literal_t *node = create_char_literal();
    node->value = exp->value;
    return (expression_node_t *) node;
}

expression_node_t *
copy_char_class(char_class_t *exp)
{
    char_class_t *node = create_char_class();
    memcpy(node->allowed_values, exp->allowed_values, 256);
    return (expression_node_t *) node;
}

expression_node_t *
copy_postfix_expression(postfix_expression_t *exp)
{
    postfix_expression_t *node = create_postfix_exp();
    node->left = copy_expression(exp->left);
    node->op = exp->op;
    return (expression_node_t *) node;
}

expression_node_t *
copy_infix_expression(infix_expression_t *exp)
{
    infix_expression_t *node = create_infix_exp();
    node->left = copy_expression(exp->left);
    node->right = copy_expression(exp->right);
    node->op = exp->op;
    return (expression_node_t *) node;
}

expression_node_t *
copy_expression(expression_node_t *exp)
{
    switch (exp->type) {
    case CHAR_LITERAL:
        return copy_char_literal((char_literal_t *) exp);
    case CHAR_CLASS:
        return copy_char_class((char_class_t *) exp);
    case POSTFIX_EXPRESSION:
        return copy_postfix_expression((postfix_expression_t *) exp);
    case INFIX_EXPRESSION:
        return copy_infix_expression((infix_expression_t *) exp);
    default:
        errx(EXIT_FAILURE, "Unsupported expression type");
    }
}
