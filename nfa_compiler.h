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

#ifndef NFA_COMPILER_H
#define NFA_COMPILER_H

#include "ast.h"

typedef struct nfa_state_t {
    struct nfa_state_t *out;
    struct nfa_state_t *out1;
    struct end_state_list *end_list;
    uint8_t c[256];
    size_t state_idx;
} nfa_state_t;

typedef struct end_state_list {
    nfa_state_t *state;
    struct end_state_list *next;
    struct end_state_list *tail;
} end_state_list;

typedef struct nfa_machine_t {
    nfa_state_t *start;
    size_t nstates;
} nfa_machine_t;


extern const nfa_state_t ACCEPTING_STATE;

#define is_end_state(s) (s == &ACCEPTING_STATE)
#define is_matching_state(s, c) (s->c[MATCH_ALL]? 1: s->c[c])
#define is_null_state(s) (s->c[NULL_STATE])

typedef nfa_state_t * (*expression_compile_fn) (nfa_machine_t *, expression_node_t *);


void free_nfa(nfa_machine_t *);
nfa_machine_t *compile_regex(const char *);
void free_end_list(end_state_list *);
#endif