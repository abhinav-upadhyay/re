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
void free_expression(expression_node_t *);

#endif