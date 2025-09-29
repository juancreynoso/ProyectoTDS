#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "ast.h"
#include "symbol_table.h"
#include "type_check.h"

typedef struct tables_stack tables_stack;
typedef struct symbol_table symbol_table;

extern int exists_main;

void semantic_analysis_recursive(node* root, tables_stack* stack, symbol_table* table, node* parent);
tables_stack* analyze_semantics(node* root);
void add_formal_params_to_scope(tables_stack* stack, Formal_P_List* f_params);
int verify_method_params(Formal_P_List* formal_params, Current_P_List* actual_params, tables_stack* stack, char* method_name);

#endif