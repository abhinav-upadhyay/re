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

#include <stdlib.h>

#include "nfa_compiler.h"
#include "nfa_executor.h"
#include "parser.h"
#include "test_utils.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


static void
test_matches(void)
{
    typedef struct test_input {
        const char *regex;
        const char *s;
        int expected;
    } test_input;

    test_input tests[] = {
        {"a*", "aa", 1},
        {"a*", "a", 1},
        {"a*", "ab", 1},
        {"a*", "ba", 1},
        {"a*", "b", 1},
        {"a+", "a", 1},
        {"a+", "aa", 1},
        {"a+", "aaa", 1},
        {"a+", "ab", 1},
        {"a+", "b", 0},
        {"a+", "ba", 0},
        {"a+", "aba", 1},
        {"a+", "aaba", 1},
        {"a+", "bbbb", 0},
        {"a?aa", "aa", 1},
        {"a?a?aa", "a", 0},
        {"a?a?aa", "aa", 1},
        {"a?", "a", 1},
        {"a?", "ab", 1},
        {"a?", "b", 1},
        {"a?a?a?aaa", "aaa", 1},
        {"a?a?a?aaa", "aaab", 1},
        {"a?a?a?aaa", "a", 0},
        {"((ab|cd)+)12", "ab12", 1},
        {"((ab|cd)+)12", "cd12", 1},
        {"((ab|cd)+)12", "12", 0},
        {"((ab|cd)+)12", "b12", 0},
        {"((ab|cd)+)12", "de12", 0},
        {"((ab|cd)+)12", "12ab", 0},
        {"((ab|cd)+)12", "ad12", 0},
        {"(a|b|c|d|e)?(1|2|3|4)+(a|b)", "a1a", 1},
        {"(a|b|c|d|e)?(1|2|3|4)+(a|b)", "b1a", 1},
        {"(a|b|c|d|e)?(1|2|3|4)+(a|b)", "c1a", 1},
        {"(a|b|c|d|e)?(1|2|3|4)+(a|b)", "d1a", 1},
        {"(a|b|c|d|e)?(1|2|3|4)+(a|b)", "e1a", 1},
        {"(a|b|c|d|e)?(1|2|3|4)+(a|b)", "a2a", 1},
        {"(a|b|c|d|e)?(1|2|3|4)+(a|b)", "a3a", 1},
        {"(a|b|c|d|e)?(1|2|3|4)+(a|b)", "a4a", 1},
        {"(a|b|c|d|e)?(1|2|3|4)+(a|b)", "a1b", 1},
        {"(a|b|c|d|e)?(1|2|3|4)+(a|b)", "a2b", 1},
        {"(a|b|c|d|e)?(1|2|3|4)+(a|b)", "e2a", 1},
        {"(a|b|c|d|e)?(1|2|3|4)+(a|b)", "c2b", 1},
        {"(a|b|c|d|e)?(1|2|3|4)+(a|b)", "aa", 0},
        {"(a|b|c|d|e)?(1|2|3|4)+(a|b)", "a1d", 0},
        {"(a|b|c|d|e)?(1|2|3|4)+(a|b)", "a3b", 1},
        {"(a|b|c|d|e)?(1|2|3|4)+(a|b)", "1a", 1},
        {"(a|b|c|d|e)?(1|2|3|4)+(a|b)", "2a", 1},
        {"(a|b|c|d|e)?(1|2|3|4)+(a|b)", "1b", 1},
        {"(a|b|c|d|e)?(1|2|3|4)+(a|b)", "2b", 1},
        {"(a|b|c|d|e)?(1|2|3|4)+(a|b)", "1", 0},
        {"a+b+c+de", "aabde", 0},
        {"a+b+c+de", "123aabcde", 0},
        {"a+b+c+de", "aabcde", 1}
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        test_input t = tests[i];
        printf("Testing regex %s with string %s---", t.regex, t.s);
        nfa_machine_t *machine = compile_regex(t.regex);
        int match = nfa_execute(machine, t.s);
        test(match == t.expected, ANSI_COLOR_RED "failed for input %s: %s\n" ANSI_COLOR_RESET, t.regex, t.s);
        printf(ANSI_COLOR_GREEN "-Passed!" ANSI_COLOR_RESET "\n");
        free_nfa(machine);
    }
}

int
main(int argc, char **argv)
{
    test_matches();
}