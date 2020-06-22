#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "token.h"
#include "test_utils.h"

typedef struct test_case {
    const char *input;
    token_t expected_tokens [100];
} test_case; 

int
main(int argc, char **argv)
{
    test_case tests [] = {
        "(ab)*(c|d)",
        {
            {LPAREN, "("},
            {CHAR, "a"},
            {CHAR, "b"},
            {RPAREN, ")"},
            {STAR, "*"},
            {LPAREN, "("},
            {CHAR, "c"},
            {PIPE, "|"},
            {CHAR, "d"},
            {RPAREN, ")"},
            {END_OF_FILE, ""}
        },
        "a|b",
        {
            {CHAR, "a"},
            {PIPE, "|"},
            {CHAR, "b"},
            {END_OF_FILE, ""}
        },
        "(ab|c)*d",
        {
            {LPAREN, "("},
            {CHAR, "a"},
            {CHAR, "b"},
            {PIPE, "|"},
            {CHAR, "c"},
            {RPAREN, ")"},
            {STAR, "*"},
            {CHAR, "d"},
            {END_OF_FILE, ""}
        },
        "ab",
        {
            {CHAR, "a"},
            {CHAR, "b"},
            {END_OF_FILE, ""}
        },
    };

    size_t ntests = sizeof(tests) / sizeof(tests[0]);
    for (size_t i = 0; i < ntests; i++) {
        test_case test_case = tests[i];
        print_test_separator_line();
        printf("Testing lexing for %s\n", test_case.input);
        lexer_t *l = lexer_init(test_case.input);
        while (l->ch != 0) {
            for (int i = 0; test_case.expected_tokens[i].type != END_OF_FILE; i++) {
                token_t *tok = next_token(l);
                test(test_case.expected_tokens[i].type == tok->type, "Expected token type [%s], found [%s]\n",
                    get_token_name(test_case.expected_tokens[i].type), get_token_name(tok->type));
                test(strcmp(test_case.expected_tokens[i].literal, tok->literal) == 0,
                    "Expected token literal %s, found %s\n",
                    test_case.expected_tokens[i].literal, tok->literal);
            }
        }
        lexer_free(l);
    }
}