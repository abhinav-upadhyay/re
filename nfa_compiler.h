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

#include "re_utils.h"


typedef enum state_type_t {
    STATE_CHAR,
    STATE_CONCAT,
    STATE_OR,
    STATE_ONE_OR_MORE,
    STATE_ZERO_OR_MORE,
    STATE_ZERO_OR_ONE
} state_type_t;

typedef struct nfa_state_t {
    struct nfa_state_t *out;
    struct nfa_state_t *out1;
    struct end_state_list *end_list;
    size_t state_idx;
    u_int8_t c[256];
} nfa_state_t;

typedef struct end_state_list {
    nfa_state_t *state;
    struct end_state_list *next;
} end_state_list;

typedef struct nfa_machine_t {
    nfa_state_t *start;
    cm_array_list *state_list;
} nfa_machine_t;

#define END_STATE 0
#define NULL_STATE 255

int is_end_state(nfa_state_t *);
int is_matching_state(nfa_state_t *, u_int8_t);
int is_null_state(nfa_state_t *);

// extern nfa_state_t ACCEPTING_STATE;

void free_nfa(nfa_machine_t *);
nfa_machine_t *compile_regex(const char *);
#endif