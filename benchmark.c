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
    size_t pattern_size = 2 * n + n +1;
    size_t string_size = n + 1;
    pattern = malloc(pattern_size + 1);
    if (pattern == NULL)
        err(EXIT_FAILURE, "malloc failed");
    string = malloc(string_size + 1);
    if (string == NULL)
        err(EXIT_FAILURE, "malloc failed");
    memset(pattern, 'a', pattern_size - 1);
    for (size_t i = 0; i < 2 * n; i++) {
        if (i % 2 == 0)
            pattern[i] = 'a';
        else
            pattern[i] = '?';
    }
    pattern[pattern_size] = 0;
    memset(string, 'a', n);
    string[n] = 0;
    // printf("pattern: %s, string: %s\n", pattern, string);
    start = clock();
    nfa_machine_t * machine = compile_regex(pattern);
    int match = nfa_execute(machine->start, string);
    end = clock();
    time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("[%d] Time for n=%zu: %f s\n", match, n, time_used);
    free(pattern);
    free(string);
    // free_nfa(machine);
}

int
main(int argc, char **argv)
{
    for (size_t i = 1; i < 30; i++)
        execute(i);
    return 0;

}
