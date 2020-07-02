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
        {
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
            }
        },
        {
            "a|b",
            {
                {CHAR, "a"},
                {PIPE, "|"},
                {CHAR, "b"},
                {END_OF_FILE, ""}
            }
        },
        {
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
            }
        },
        {
            "ab",
            {
                {CHAR, "a"},
                {CHAR, "b"},
                {END_OF_FILE, ""}
            }
        },
        {
            "[a-z]",
            {
                {LBRACKET, "["},
                {CHAR, "a"},
                {CHAR, "-"},
                {CHAR, "z"},
                {RBRACKET, "]"},
                {END_OF_FILE, ""}
            }
        },
        {
            "a?[0-9]+",
            {
                {CHAR, "a"},
                {QUESTION, "?"},
                {LBRACKET, "["},
                {CHAR, "0"},
                {CHAR, "-"},
                {CHAR, "9"},
                {RBRACKET, "]"},
                {PLUS, "+"},
                {END_OF_FILE, ""}
            }
        }
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
                token_free(tok);
            }
        }
        lexer_free(l);
    }
}