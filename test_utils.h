#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <assert.h>
#include <stdio.h>

#define test(expr, ...) if (expr) { \
    ; } else { \
    fprintf(stderr, __VA_ARGS__); \
    abort(); \
    }

void
print_test_separator_line(void)
{
    for (int i = 0; i < 100; i++)
        printf("-");
    printf("\n");
}

#endif