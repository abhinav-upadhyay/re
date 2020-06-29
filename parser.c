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
static char * to_string(node_t *);
static expression_node_t * parse_infix_expression(parser_t *, expression_node_t *);
static expression_node_t * parse_postfix_expression(parser_t *, expression_node_t *);
static expression_node_t * parse_char_node(parser_t *);
static expression_node_t * parse_re_group(parser_t *);
static void print_exp(expression_node_t *, size_t);

static prefix_parse_fn prefix_fns[] = {
    parse_char_node, // char
    NULL, // dot
    NULL, // plus
    NULL, // question
    NULL, // pipe
    NULL, // cat
    parse_re_group, // lparen
    NULL, // rparen
    NULL, // star
    NULL, // illegal
    NULL, // EOF
};

static postfix_parse_fn postfix_fns[] = {
    NULL, // char
    NULL, // dot
    parse_postfix_expression, // plus
    parse_postfix_expression, // question
    NULL, // pipe
    NULL, // cat
    NULL, // lparen
    NULL, // rparen
    parse_postfix_expression, // star
    NULL, // illegal
    NULL // EOF
};

static infix_parse_fn infix_fns[] = {
    parse_infix_expression, // char
    NULL, // dot
    NULL, // plus
    NULL, // question
    parse_infix_expression, // pipe
    parse_infix_expression, // cat
    parse_infix_expression, // lparen
    NULL, // rparen
    NULL, // star
    NULL, // illegal
    NULL // EOF
};

static operator_precedence_t
get_precedence(token_type toktype)
{
    switch (toktype) {
        case PIPE:
            return PRE_OR;
        case CAT:
            return PRE_CAT;
        case PLUS:
        case QUESTION:
        case STAR:
            return PRE_ASTERISK;
        default:
            return LOWEST;    
    }
}

static operator_precedence_t
peek_precedence(parser_t *parser)
{
    if (parser->peek_tok->type == CHAR || parser->peek_tok->type == LPAREN)
        return get_precedence(CAT);
    else
        return get_precedence(parser->peek_tok->type);;
}


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

    if (parser->cur_tok) // && parser->cur_tok->type != END_OF_FILE)
        token_free(parser->cur_tok);
    // if (parser->cur_tok->type == END_OF_FILE)
        // return;
    parser->cur_tok = parser->peek_tok;
    parser->peek_tok = next_token(parser->lexer);
}

static operator_t
get_op(token_type toktype)
{
    switch (toktype) {
    case CHAR:
    case DOT:
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
    
    while (parser->peek_tok->type != terminator_tok) {
        if (parser->peek_tok->type == terminator_tok)
            break;
        if (precedence >= peek_precedence(parser))
            break;
        infix_parse_fn infix_fn = infix_fns[parser->peek_tok->type];
        if (infix_fn != NULL) {
            parser_next_token(parser);
            expression_node_t *right = infix_fn(parser, left);
            left = right;
            // parser_next_token(parser);
            continue;
        }
        postfix_parse_fn postfix_fn = postfix_fns[parser->peek_tok->type];
        if (postfix_fn != NULL) {
            parser_next_token(parser);
            expression_node_t *right = postfix_fn(parser, left);
            left = right;
            // parser_next_token(parser);
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
    postfix_exp->expression.node.type = EXPRESSION_NODE;
    postfix_exp->expression.node.string = to_string;
    return postfix_exp;
}

static expression_node_t *
parse_postfix_expression(parser_t *parser, expression_node_t *left)
{
    postfix_expression_t *postfix_exp = create_postfix_exp();
    postfix_exp->expression.node.token = token_copy(parser->cur_tok);
    // postfix_exp->expression.node.token_literal = strdup(parser->cur_tok->literal);
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
    infix_exp->expression.node.type = EXPRESSION_NODE;
    infix_exp->expression.node.string = to_string;
    return infix_exp;
}

static expression_node_t *
parse_infix_expression(parser_t *parser, expression_node_t *left)
{
    operator_precedence_t precedence;
    infix_expression_t *infix_exp = create_infix_exp();
    infix_exp->expression.node.token = token_copy(parser->cur_tok);
    // infix_exp->expression.node.token_literal = strdup(parser->cur_tok->literal);
    infix_exp->left = left;
    //?? should we call parse_expression here? It should somehow not parse too much, how to stop?
    // we can go case by case - if next token is char, just parse it. If next token is
    token_type terminating_tok = parser->cur_tok->type == LPAREN? RPAREN: END_OF_FILE;
    if (parser->cur_tok->type == CHAR_LITERAL || parser->cur_tok->type == LPAREN) {
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
    // parser_next_token(parser);
    return exp;
}

char_literal_t *
create_char_literal(void)
{
    char_literal_t *char_node;
    char_node = malloc(sizeof(*char_node));
    if (char_node == NULL)
        err(EXIT_FAILURE, "malloc failed");
    char_node->expression.type = CHAR_LITERAL;
    char_node->expression.node.type = EXPRESSION_NODE;
    char_node->expression.node.string = to_string;
    return char_node;
}

static expression_node_t *
parse_char_node(parser_t *parser)
{
    char_literal_t *char_node = create_char_literal();
    char_node->expression.node.token = token_copy(parser->cur_tok);
    // char_node->expression.node.token_literal = strdup(parser->cur_tok->literal);
    char_node->value = parser->cur_tok->literal[0];
    return (expression_node_t *) char_node;
}


regex_t *
parse_regex(parser_t *parser)
{
    regex_t * regex = regex_init();
    node_t *node = (node_t *) parse_expression(parser, LOWEST, END_OF_FILE);
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



static void
free_infix_expression(infix_expression_t *exp)
{
    free_expression(exp->left);
    free_expression(exp->right);
    free(exp);
}

static void
free_postfix_expression(postfix_expression_t *exp)
{
    free_expression(exp->left);
    free(exp);
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
    token_free(exp->node.token);
    // free(exp->node.token_literal);
    switch (exp->type) {
    case CHAR_LITERAL:
        free(exp);
        return;
    case INFIX_EXPRESSION:
        free_infix_expression((infix_expression_t *) exp);
        return;
    case POSTFIX_EXPRESSION:
        free_postfix_expression((postfix_expression_t *) exp);
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
    default:
        return NULL;
    }
}

static char *
to_string(node_t *node)
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
