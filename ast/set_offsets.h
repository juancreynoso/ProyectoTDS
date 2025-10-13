// set_offsets.h
#ifndef SET_OFFSETS_H
#define SET_OFFSETS_H

#include "../common.h"
#include "ast.h"

void reset_offsets();
int new_var_offset();
int new_param_offset();

void set_offsets(node* root);

#endif