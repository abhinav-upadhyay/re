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
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "nfa_compiler.h"
#include "nfa_executor.h"

#define LIST_SIZE 128
static nfa_state_t ** clist;
static nfa_state_t ** nlist;
static uint8_t *idx_list;
static size_t CLIST_INDEX;
static size_t NLIST_INDEX;
static size_t counter = 0;

#define add_state_to_list(s) if (idx_list[s->state_idx] <= counter) { \
    idx_list[s->state_idx]++; \
    nlist[NLIST_INDEX++] = s; \
    }

static void
find_match_state(nfa_state_t *s, u_int8_t c)
{
    if (is_end_state(s)) {
        add_state_to_list(s);
        return;
    }

    if (is_null_state(s)) {
        find_match_state(s->out, c);
        if (s->out1)
            find_match_state(s->out1, c);
        return;
    }

    if (!is_matching_state(s, c))
        return;
    add_state_to_list(s);
}

static int
check_end_state(nfa_state_t *s)
{
    if (is_end_state(s))
        return 1;
    if (is_end_state(s->out))
        return 1;
    if (s->out1 && is_end_state(s->out1))
        return 1;

    if (is_null_state(s->out)) {
        if (is_end_state(s->out->out1) || is_end_state(s->out->out))
            return 1;
    }
    return 0;
}

int
nfa_execute(nfa_machine_t *machine, const char *string)
{
    clist = calloc(LIST_SIZE, sizeof(nfa_state_t *));
    nlist = calloc(LIST_SIZE, sizeof(nfa_state_t *));
    size_t idx_len = machine->nstates;
    idx_list = calloc(1, idx_len);
    CLIST_INDEX = 0;
    NLIST_INDEX = 0;
    int retval = 0;
    counter = 0;
    find_match_state(machine->start, *string++);
    counter++;
    CLIST_INDEX = NLIST_INDEX;
    NLIST_INDEX = 0;
    nfa_state_t **temp_list = nlist;
    nlist = clist;
    clist = temp_list;

    while (*string) {
        u_int8_t c = (u_int8_t) *string++;
        for (size_t i = 0; i < CLIST_INDEX; i++) {
            nfa_state_t *s = clist[i];
            if (is_end_state(s)) {
                add_state_to_list(s);
                continue;
            }
            if (is_end_state(s->out)) {
                add_state_to_list(s->out);
            } else {
                find_match_state(s->out, c);
            }
            if (s->out1) {
                if (is_end_state(s->out1)) {
                    add_state_to_list(s->out1);
                } else {
                    find_match_state(s->out1, c);
                }
            }
        }
        counter++;
        CLIST_INDEX = NLIST_INDEX;
        NLIST_INDEX = 0;
        temp_list = nlist;
        nlist = clist;
        clist = temp_list;
        if (CLIST_INDEX == 0)
            break;
    }
    for (size_t i = 0; i < CLIST_INDEX; i++) {
        if (check_end_state(clist[i])) {
            retval = 1;
            break;
        }
    }
    free(idx_list);
    free(clist);
    free(nlist);
    return retval;
}
