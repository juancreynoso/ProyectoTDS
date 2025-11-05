// set_offsets.h
#ifndef SET_OFFSETS_H
#define SET_OFFSETS_H

#include <stdio.h>
#include <stdlib.h>
#include "../common.h"
#include "ast.h"

typedef struct offset_list {
    int info;
    struct offset_list* next;
} offset_list;

void reset_offsets();
int new_var_offset(int opt);
int new_param_offset();
int get_frame_size();

void set_offsets(node* root, int opt);
void set_offsets_actual_params(node* root, int opt);

offset_list* init_offset_list();
int is_empty(offset_list *list);
void add_offset(offset_list **list, int offset);
int get_offset(offset_list **list);
void print_offsets(offset_list *list);

#endif