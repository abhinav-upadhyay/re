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

#include "ast.h"
#include "parser.h"
#include "lexer.h"
#include "nfa_compiler.h"
#include "re_utils.h"

static nfa_state_t * compile_expression_node(nfa_machine_t *, expression_node_t *);
static size_t state_counter = 0;

// nfa_state_t ACCEPTING_STATE = {END_STATE, NULL, NULL};
#define ACCEPTING_STATE(machine) create_state(machine, END_STATE)

static nfa_state_t *
create_state(nfa_machine_t *machine, int c)
{
    nfa_state_t *state;
    state = malloc(sizeof(*state));
    if (state == NULL)
        err(EXIT_FAILURE, "malloc failed");
    state->c = c;
    state->out1 = NULL;
    state->out = NULL;
    if (c != END_STATE) {
        state->end_list = malloc(sizeof(end_state_list));
        if (state->end_list == NULL)
            err(EXIT_FAILURE, "malloc failed");
        state->end_list->state = NULL;
        state->end_list->next = NULL;
    } else {
        state->end_list = NULL;
    }
    cm_array_list_add(machine->state_list, state);
    state->state_idx = machine->state_list->length;
    return state;
}

static void
free_end_list(end_state_list *list)
{
    if (list == NULL)
        return;
    end_state_list *node = list;
    end_state_list *temp;
    while (node) {
        temp = node->next;
        free(node);
        node = temp;
    }
}

static nfa_state_t *
compile_infix_node(nfa_machine_t *machine, infix_expression_t *node)
{
    nfa_state_t *left = compile_expression_node(machine, node->left);
    nfa_state_t *right = compile_expression_node(machine, node->right);
    if (node->op == OR) {
        nfa_state_t *state = create_state(machine, NULL_STATE);
        state->out = left;
        state->out1 = right;
        free_end_list(state->end_list);
        state->end_list = left->end_list;
        end_state_list *temp = state->end_list;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = right->end_list;
        left->end_list = NULL;
        right->end_list = NULL;
        return state;
    } else if (node->op == CONCAT) {
        end_state_list *temp = left->end_list;
        while (temp != NULL) {
            if (temp->state->out && temp->state->out->c == END_STATE) {
                temp->state->out = right;
            }
            if (temp->state->out1 && temp->state->out1->c == END_STATE) {
                temp->state->out1 = right;
            }
            temp = temp->next;
        }
        free_end_list(left->end_list);
        left->end_list = right->end_list;
        right->end_list = NULL;
        return left;
    }
    return NULL; // not expected to come here, only two infix operators supported
}



static nfa_state_t *
compile_postfix_node(nfa_machine_t *machine, postfix_expression_t *node)
{
    nfa_state_t *state = create_state(machine, NULL_STATE);
    nfa_state_t *left = compile_expression_node(machine, node->left);
    if (node->op == ZERO_OR_ONE) {
        state->out = left;
        state->out1 = ACCEPTING_STATE(machine);
        state->end_list->state = state;
        state->end_list->next = left->end_list;
        left->end_list = NULL; //TODO? check
        return state;
    } else if (node->op == ZERO_OR_MORE) {
        state->out = left;
        end_state_list *temp = left->end_list;
        while (temp != NULL) {
            if (temp->state->out && temp->state->out->c == END_STATE) {
                temp->state->out = state;
            }
            if (temp->state->out1 && temp->state->out1->c == END_STATE) {
                temp->state->out1 = state;
            }
            temp = temp->next;
        }
        free_end_list(left->end_list);
        left->end_list = NULL;
        state->end_list->state = state;
        state->out1 = ACCEPTING_STATE(machine);
        return state;
    } else if (node->op == ONE_OR_MORE) {
        end_state_list *temp = left->end_list;
        while (temp) {
            if (temp->state->out && temp->state->out->c == END_STATE) {
                temp->state->out = state;
            }
            if (temp->state->out1 && temp->state->out1->c == END_STATE) {
                temp->state->out1 = state;
            }
            temp = temp->next;
        }
        state->out = left;
        state->out1 = ACCEPTING_STATE(machine);
        free_end_list(left->end_list);
        left->end_list = state->end_list;
        left->end_list->state = state;
        state->end_list = NULL;
        return left;
    }
    return NULL; //should not reach here
}

static nfa_state_t *
compile_char_node(nfa_machine_t *machine, char_literal_t *node){
    nfa_state_t *state = create_state(machine, node->value);
    state->out = ACCEPTING_STATE(machine);
    state->out1 = NULL;
    state->end_list->state = state;
    return state;
}


static nfa_state_t *
compile_expression_node(nfa_machine_t *machine, expression_node_t *node)
{
    switch (node->type) {
    case INFIX_EXPRESSION:
        return compile_infix_node(machine, (infix_expression_t *) node);
    case POSTFIX_EXPRESSION:
        return compile_postfix_node(machine, (postfix_expression_t *) node);
    case CHAR_LITERAL:
        return compile_char_node(machine, (char_literal_t *) node);
    default:
        errx(EXIT_FAILURE, "Unsupported node type");
    }
}

nfa_machine_t *
compile_regex(const char *regex_pattern)
{
    lexer_t *lexer = lexer_init(regex_pattern);
    parser_t *parser = parser_init(lexer);
    regex_t *regex = parse_regex(parser);
    nfa_machine_t *machine;
    machine = malloc(sizeof(*machine));
    if (machine == NULL)
        err(EXIT_FAILURE, "malloc failed");
    machine->state_list = cm_array_list_init(128, free);
    nfa_state_t *compiled_regex = compile_expression_node(machine, (expression_node_t *) regex->root);
    parser_free(parser);
    regex_free(regex);
    machine->start = compiled_regex;
    return machine;
}


void
free_nfa(nfa_machine_t *machine)
{
    for (size_t i = 0; i < machine->state_list->length; i++) {
        nfa_state_t *s = machine->state_list->array[i];
        if (s->end_list)
            free_end_list(s->end_list);
        free(s);
    }
    free(machine->state_list->array);
    free(machine->state_list);
    free(machine);
}