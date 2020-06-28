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
    int c;
} nfa_state_t;

typedef struct end_state_list {
    nfa_state_t *state;
    struct end_state_list *next;
} end_state_list;

typedef struct nfa_machine_t {
    nfa_state_t *start;
    cm_array_list *state_list;
} nfa_machine_t;

#define START_STATE -1
#define END_STATE 0
#define NULL_STATE 256

// extern nfa_state_t ACCEPTING_STATE;

void free_nfa(nfa_machine_t *);
nfa_machine_t *compile_regex(const char *);
#endif