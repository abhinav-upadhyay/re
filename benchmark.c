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
#include <time.h>

#include "nfa_compiler.h"
#include "nfa_executor.h"

static void
execute(size_t n)
{
    char *pattern, *string;
    clock_t start, end;
    double time_used;
    size_t pattern_size = 2 * n + n + 1;
    size_t string_size = n + 1;
    pattern = malloc(pattern_size);
    if (pattern == NULL)
        err(EXIT_FAILURE, "malloc failed");
    string = malloc(string_size);
    if (string == NULL)
        err(EXIT_FAILURE, "malloc failed");
    memset(pattern, 'a', pattern_size - 1);
    for (size_t i = 0; i < 2 * n; i++) {
        if (i % 2 == 0)
            pattern[i] = 'a';
        else
            pattern[i] = '?';
    }
    pattern[pattern_size - 1] = 0;
    memset(string, 'a', n);
    string[n] = 0;
    start = clock();
    nfa_machine_t * machine = compile_regex(pattern);
    int match = nfa_execute(machine, string);
    free(pattern);
    free(string);
    free_nfa(machine);
    end = clock();
    time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("%zu,%f\n", n, time_used);
}

int
main(int argc, char **argv)
{
    for (size_t i = 1; i < 100; i++)
        execute(i);
    return 0;

}
