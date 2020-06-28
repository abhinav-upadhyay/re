#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "test_utils.h"

static int compare_expression(expression_node_t *, expression_node_t *);

static expression_node_t *
postfix_node(expression_node_t *left, operator_t op)
{
    postfix_expression_t *postfix_exp = create_postfix_exp();
    postfix_exp->left = left;
    postfix_exp->op = op;
    return (expression_node_t *) postfix_exp;
}

static expression_node_t *
infix_node(expression_node_t *left, expression_node_t *right, operator_t op)
{
    infix_expression_t *exp = create_infix_exp();
    exp->left = left;
    exp->right = right;
    exp->op = op;
    return (expression_node_t *) exp;
}

static expression_node_t *
char_node(char c)
{
    char_literal_t *char_exp = create_char_literal();
    char_exp->value = c;
    return (expression_node_t *) char_exp;
}

static int
compare_char_node(char_literal_t *expected, char_literal_t *actual)
{
    return expected->value == actual->value;
}

static int
compare_postfix_expression(postfix_expression_t *expected, postfix_expression_t *actual)
{
    return expected->op == actual->op && compare_expression(expected->left, actual->left);
}

static int
compare_infix_expression(infix_expression_t *expected, infix_expression_t *actual)
{
    return expected->op == actual->op && compare_expression(expected->left, actual->left) &&
        compare_expression(expected->right, actual->right);
}

static int
compare_expression(expression_node_t *expected, expression_node_t *actual)
{
    if (expected->type != actual->type)
        return 0;
    
    switch (expected->type) {
    case INFIX_EXPRESSION:
        return compare_infix_expression((infix_expression_t *) expected, (infix_expression_t *) actual);
    case POSTFIX_EXPRESSION:
        return compare_postfix_expression((postfix_expression_t *) expected, (postfix_expression_t *) actual);
    case CHAR_LITERAL:
        return compare_char_node((char_literal_t *) expected, (char_literal_t *) actual);
    default:
        return 0;
    }
}

static void
test_char_literal(void)
{
    lexer_t *lexer;
    parser_t *parser;
    regex_t *regex;
    print_test_separator_line();
    const char *input = "a";
    printf("Testing input %s\n", input);
    lexer = lexer_init(input);
    parser = parser_init(lexer);
    regex = parse_regex(parser);
    test(regex->root->type == EXPRESSION_NODE, "Expected an expression node, got %s\n", node_to_string(regex->root->type));
    expression_node_t *root = (expression_node_t *) regex->root;
    test(root->type == CHAR_LITERAL, "Expected a char literal, got %s\n", expression_type_to_string(root->type));
    char_literal_t *char_lit = (char_literal_t *) root;
    test(char_lit->value == 'a', "Expected value a, got %c\n", char_lit->value);
    parser_free(parser);
    regex_free(regex);
}

static void
test_multichar_literal(void)
{
    lexer_t *lexer;
    parser_t *parser;
    regex_t *regex;
    print_test_separator_line();
    const char *input = "ab";
    printf("Testing input %s\n", input);
    lexer = lexer_init(input);
    parser = parser_init(lexer);
    regex = parse_regex(parser);
    test(regex->root->type == EXPRESSION_NODE, "Expected an expression node, got %s\n", node_to_string(regex->root->type));
    expression_node_t *root = (expression_node_t *) regex->root;
    test(root->type == INFIX_EXPRESSION, "Expected an infix expression, got %s\n", expression_type_to_string(root->type));
    infix_expression_t *infix_exp = (infix_expression_t *) root;
    test(infix_exp->left->type == CHAR_LITERAL, "Expected char literal as left node of infix exp, got %s\n", expression_type_to_string(infix_exp->left->type));
    test(infix_exp->right->type == CHAR_LITERAL, "Expected char literal as right node of infix exp, got %s\n", expression_type_to_string(infix_exp->right->type));
    test(infix_exp->op == CONCAT, "Expected CONCAT operator, got %s\n", operator_to_string(infix_exp->op));
    char_literal_t *left = (char_literal_t *) infix_exp->left;
    test(left->value == 'a', "Expected value a, got %c\n", left->value);
    char_literal_t *right = (char_literal_t *) infix_exp->right;
    test(right->value == 'b', "Expected value b, got %c\n", right->value);
    parser_free(parser);
    regex_free(regex);
}

static void
test_simple_or(void)
{
    lexer_t *lexer;
    parser_t *parser;
    regex_t *regex;
    print_test_separator_line();
    const char *input = "a|b";
    printf("Testing input %s\n", input);
    lexer = lexer_init(input);
    parser = parser_init(lexer);
    regex = parse_regex(parser);
    test(regex->root->type == EXPRESSION_NODE, "Expected an expression node, got %s\n", node_to_string(regex->root->type));
    expression_node_t *root = (expression_node_t *) regex->root;
    test(root->type == INFIX_EXPRESSION, "Expected an infix expression, got %s\n", expression_type_to_string(root->type));
    infix_expression_t *infix_exp = (infix_expression_t *) root;
    test(infix_exp->left->type == CHAR_LITERAL, "Expected char literal as left node of infix exp, got %s\n", expression_type_to_string(infix_exp->left->type));
    test(infix_exp->right->type == CHAR_LITERAL, "Expected char literal as right node of infix exp, got %s\n", expression_type_to_string(infix_exp->right->type));
    test(infix_exp->op == OR, "Expected OR operator, got %s\n", operator_to_string(infix_exp->op));
    char_literal_t *left = (char_literal_t *) infix_exp->left;
    test(left->value == 'a', "Expected value a, got %c\n", left->value);
    char_literal_t *right = (char_literal_t *) infix_exp->right;
    test(right->value == 'b', "Expected value b, got %c\n", right->value);
    parser_free(parser);
    regex_free(regex);
}

static void
test_simple_repitition(void)
{
    typedef struct test_input {
        const char *input;
        expression_node_t *expected_exp;
    } test_input ;
    print_test_separator_line();

    test_input tests[] = {
        {
            "a*",
            postfix_node(char_node('a'), ZERO_OR_MORE)
        },
        {
            "ab+",
            infix_node(char_node('a'), postfix_node(char_node('b'), ONE_OR_MORE), CONCAT)
        },
        {
            "a+b",
            infix_node(postfix_node(char_node('a'), ONE_OR_MORE), char_node('b'), CONCAT)
        },
        {
            "a(bc+)",
            infix_node(char_node('a'), infix_node(char_node('b'), postfix_node(char_node('c'), ONE_OR_MORE), CONCAT), CONCAT)
        },
        {
            "a|(bc+)|d",
            infix_node(infix_node(char_node('a'), infix_node(char_node('b'), postfix_node(char_node('c'), ONE_OR_MORE), CONCAT) ,OR), char_node('d'), OR)
        },
        {
            "(ab)?c",
            infix_node(postfix_node(infix_node(char_node('a'), char_node('b'), CONCAT), ZERO_OR_ONE), char_node('c'), CONCAT)
        },
        {
            "((ab)|(ac))?d",
            infix_node(postfix_node(infix_node(infix_node(char_node('a'), char_node('b'), CONCAT), infix_node(char_node('a'), char_node('c'), CONCAT) , OR), ZERO_OR_ONE), char_node('d'), CONCAT)
        },
        {
            "a?aa",
            infix_node(infix_node(postfix_node(char_node('a'), ZERO_OR_ONE), char_node('a'), CONCAT), char_node('a'), CONCAT)

        },
        {
            "a?a?a",
            infix_node(infix_node(postfix_node(char_node('a'), ZERO_OR_ONE), postfix_node(char_node('a'), ZERO_OR_ONE), CONCAT), char_node('a'), CONCAT)
        }
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        test_input t = tests[i];
        printf("Testing %s\n", t.input);
        lexer_t *lexer;
        parser_t *parser;
        regex_t *regex;
        const char *input = t.input;
        lexer = lexer_init(input);
        parser = parser_init(lexer);
        regex = parse_regex(parser);
        test(regex->root->type == EXPRESSION_NODE, "Expected an expression node, got %s\n", node_to_string(regex->root->type));
        expression_node_t *root = (expression_node_t *) regex->root;
        char *expected_string = t.expected_exp->node.string((node_t *) t.expected_exp);
        char *actual_string = regex->root->string(regex->root);
        test(compare_expression(t.expected_exp, root), "Expected expression %s, got %s\n", expected_string, actual_string);
        free(expected_string);
        free(actual_string);
        parser_free(parser);
        regex_free(regex);
    }

}

int
main(int argc, char **argv)
{
    test_char_literal();
    test_multichar_literal();
    test_simple_or();
    test_simple_repitition();
    return 0;
}